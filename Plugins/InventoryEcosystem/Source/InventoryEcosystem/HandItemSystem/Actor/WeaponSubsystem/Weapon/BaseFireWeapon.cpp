// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseFireWeapon.h"

#include "TimerManager.h"
#include "InventoryEcosystem/InventoryTools/Component/MeshManager.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Bulit/BaseBulit.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/WeaponModule/BaseWeaponModule.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/WeaponModule/BaseWeaponScope.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ItemServices/BaseWeaponModuleService.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ItemServices/BaseWeaponService.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/StaticMesh.h"
#include "InventoryEcosystem/HandItemSystem/Animation/Weapon/BaseWeaponAnimationInstance.h"

// Sets default values
ABaseFireWeapon::ABaseFireWeapon()
{
	MeshManager = CreateDefaultSubobject<UMeshManager>(TEXT("MeshManager"));
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseFireWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetFireWeaponData()) return;
}

void ABaseFireWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentSettingControll = DefaultSettingControll;

	if (GetFireWeaponData())
	{
		CurrentFireType = GetFireWeaponData()->FireTypeByDefault;
		CurrentFireSound = GetFireWeaponData()->OnShotFireSound;
		CurrentUseRateDelay = GetFireWeaponData()->UseRateDelay;

		if (IsNetMode(ENetMode::NM_DedicatedServer))
		{
			GetWeaponMesh()->SetSkeletalMesh(GetItem()->GetWorldPlacementService()->GetSkeletalMesh().LoadSynchronous());
		}
		GetWeaponMesh()->SetRelativeLocationAndRotation(GetFireWeaponData()->LocationMeshOffset, GetFireWeaponData()->RotationMeshOffset);
	}
	if (GetItem())
	{
		UBaseWeaponService* WeaponService = GetItem()->GetServiceTyped<UBaseWeaponService>(UBaseWeaponService::StaticClass());
		CanBeFire = WeaponService->HasModuleByTag(WeaponModificationType::Muzzle);
	}
	RefreshAttachment();
}

void ABaseFireWeapon::InitWeapon(UBaseWeaponData* InWeaponData)
{
	Super::InitWeapon(InWeaponData);

	OnRep_WeaponData();

	if (GetFireWeaponData())
	{
		if (IsNetMode(ENetMode::NM_DedicatedServer))
		{
			GetWeaponMesh()->SetSkeletalMesh(GetItem()->GetWorldPlacementService()->GetSkeletalMesh().LoadSynchronous());
		}
		GetWeaponMesh()->SetRelativeLocationAndRotation(GetFireWeaponData()->LocationMeshOffset, GetFireWeaponData()->RotationMeshOffset);

		FMeshComponentData ComponentData;
		ComponentData.MeshRef = GetItem()->GetWorldPlacementService()->GetSkeletalMesh();
		MeshManager->AddMesh(ComponentData);
	}

	if (HasAuthority() && GetFireWeaponData())
	{
		HasTacticalAmmo = GetFireWeaponData()->bHasTacticalAmmo;
	}
}


void ABaseFireWeapon::BeginDestroy()
{
	for (auto Mod : Mods)
	{
		if (!IsValid(Mod.ActorModuleInfo)) continue;
		Mod.ActorModuleInfo->Destroy();
	}
	Super::BeginDestroy();
}
#pragma endregion //Engine

#pragma region Replicated

void ABaseFireWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseFireWeapon, CurrentOptic);
	DOREPLIFETIME(ABaseFireWeapon, CurrentFireType);
	DOREPLIFETIME(ABaseFireWeapon, CanBeFire);
	DOREPLIFETIME(ABaseFireWeapon, bIsCharged);
}

#pragma endregion //Replicated

#pragma region StartFire

bool ABaseFireWeapon::Fire(bool IsUse)
{
	if (!CanFire()) return false;

	bIsFire = IsUse;
	
	if (bIsFire)
	{
		if (NeedToReload()) { Reload(); return false; }

		switch (CurrentFireType)
		{
		case EWeaponFireType::EWFT_Single:
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(Timer_FireDelay)) return false;

			GetWorld()->GetTimerManager().SetTimer(Timer_FireDelay, ([&]() { }), (60.f / CurrentUseRateDelay), false);
			Shooting();
			return true;
		}
		case EWeaponFireType::EWFT_Burst:
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(Timer_BurstFire)) return true;
			BurstCounter = 0;
			BurstFire();
			if (GetNetMode() == ENetMode::NM_DedicatedServer || GetNetMode() == ENetMode::NM_ListenServer)
				Multicast_BirstFire();
			return true;
		}
		case EWeaponFireType::EWFT_Auto:
		{
			if (GetNetMode() == ENetMode::NM_DedicatedServer || GetNetMode() == ENetMode::NM_ListenServer)
			{
				Multicast_ChangeFireState(bIsFire);
			}
			GetWorld()->GetTimerManager().SetTimer(Timer_AutoFire, ([&]() {Shooting(); }), (60.f / CurrentUseRateDelay), true);
			if (Shooting())
			{
				OnWeaponFireStatus.Broadcast(this, true);
			}
			return true;

		}
		default: return false;
		}

	}
	else
	{
		OnWeaponFireStatus.Broadcast(this, false);
		NumShot = 0;
		if (GetNetMode() == ENetMode::NM_DedicatedServer || GetNetMode() == ENetMode::NM_ListenServer)
		{
			Multicast_ChangeFireState(bIsFire);
		}

		GetWorld()->GetTimerManager().ClearTimer(Timer_AutoFire);
	}
	return true;
}

bool ABaseFireWeapon::BurstFire()
{
	if (!GetFireWeaponData()) return false;
	if (BurstCounter == 0 && !GetWorld()->GetTimerManager().IsTimerActive(Timer_BurstFire))
	{
		GetWorld()->GetTimerManager().SetTimer(Timer_BurstFire, ([&]() {BurstFire(); }), (60.f / CurrentUseRateDelay), true);
	}
	Shooting();
	BurstCounter++;

	if (BurstCounter == GetFireWeaponData()->AmountPerBurst)
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_BurstFire);
		Timer_BurstFire.Invalidate();
		return true;
	}

	return true;
}

void ABaseFireWeapon::Multicast_BirstFire_Implementation()
{
	if (IsLocallyControlled()) return;
	BurstCounter = 0;
	BurstFire();
}


#pragma endregion //StartFire

#pragma region Reload

bool ABaseFireWeapon::Reload()
{
	if (!GetFireWeaponData()) return false;

	if (bIsReload) return true;

	if (GetItem())
	{
		if (UBaseWeaponService* LWeaponService = GetItem()->GetServiceTyped<UBaseWeaponService>())
		{
			if (LWeaponService->GetAmountAmmo() < 1) return false;
		}
	}

	if (GetCharacterOwner() && GetCharacterOwner()->IsLocallyControlled())
	{
		if (!HasAuthority())
		{
			Server_Reload();
		}
	}


	bIsReload = true;
	UAnimMontage* TargetMontagne = nullptr;
	UAnimMontage* TargetMontagneWeapon = nullptr;
	if (GetFireWeaponData()->bIsCircleReload)
	{
		TargetMontagne = GetFireWeaponData()->CharacterCircleChargeStart;
		TargetMontagneWeapon = GetFireWeaponData()->WeaponCircleChargeStart;
	}
	else 
	{
		TargetMontagne = (GetCurrentAmmo() < 1 ? GetFireWeaponData()->CharacterFullReload : GetFireWeaponData()->CharacterHotReload);
		TargetMontagneWeapon = (GetCurrentAmmo() < 1 ? GetFireWeaponData()->WeaponFullReload : GetFireWeaponData()->WeaponHotReload);
	}

	if (GetAnimationInstace())
	{
		GetAnimationInstace()->Montage_Play(TargetMontagneWeapon);
	}
	float timeMontagne = GetCharacterAnimInstance()->Montage_Play(TargetMontagne);

	if (GetFireWeaponData()->bIsCircleReload)
	{
		timeMontagne = (timeMontagne - (timeMontagne / 10));

		GetWorld()->GetTimerManager().SetTimer(Timer_StartWeaponReload, this, &ABaseFireWeapon::LoopReload, timeMontagne, true);
	}
	OnWeaponStartReload.Broadcast(this);

	return true;
}

void ABaseFireWeapon::LoopReload()
{
	bool CanNextCharge = true;
	if (GetItem())
	{
		if (UBaseWeaponService* LWeaponService = GetItem()->GetServiceTyped<UBaseWeaponService>())
		{
			CanNextCharge = (LWeaponService->GetAmountAmmo() > 0);
		}
		else
		{
			CanNextCharge = false;
		}
	}
	CanNextCharge &= (GetCurrentAmmo() < GetFireWeaponData()->MaxAmmo);

	UAnimMontage* TargetMontagne = (CanNextCharge ? GetFireWeaponData()->CharacterCircleChargeLoop : GetFireWeaponData()->CharacterCircleChargeExit);
	UAnimMontage* TargetMontagneWeapon = (CanNextCharge ? GetFireWeaponData()->WeaponCircleChargeLoop : GetFireWeaponData()->WeaponCircleChargeExit);

	float timeMontagne = GetCharacterAnimInstance()->Montage_Play(TargetMontagne);

	if(GetAnimationInstace())
		GetAnimationInstace()->Montage_Play(TargetMontagneWeapon);
	if (CanNextCharge)
	{
		GetWorld()->GetTimerManager().SetTimer(Timer_StartWeaponReload, this, &ABaseFireWeapon::LoopReload, timeMontagne, true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(Timer_StartWeaponReload, this, &ABaseFireWeapon::ExitReload, timeMontagne, true);
	}

}
void ABaseFireWeapon::ExitReload()
{
	bIsReload = false;
}
void ABaseFireWeapon::Server_Reload_Implementation()
{
	Reload();
}



void ABaseFireWeapon::WeaponWasReload()
{
	if (HasAuthority())
	{
		if (GetWeaponService())
		{
			int32 Ammo = (GetWeaponService()->GetCurrentAmmo() + GetWeaponService()->GetAmmo((GetWeaponService()->GetMaxAmmo() - GetWeaponService()->GetCurrentAmmo())));
			GetWeaponService()->SetCurrentAmmo(Ammo);
		}
		SetIsCharged();
		OnWeaponWasReload.Broadcast(this);
		OnChangeAmmo.Broadcast(this);
	}
	if (!GetFireWeaponData()->bIsCircleReload)
	{
		ExitReload();
	}
}
#pragma endregion //Reload

#pragma region AIM

bool ABaseFireWeapon::I_UnequipActor_Implementation()
{
    return Super::I_UnequipActor_Implementation();
}

bool ABaseFireWeapon::Aim(bool IsUse)
{	
	IsAiming = IsUse;
	if (CurrentOptic)
	{
		CurrentOptic->SetActiveRender(IsUse);
	}
	return true;
}


bool ABaseFireWeapon::I_SwitchScope_Implementation(bool Next)
{
	if (Mods.Num() == 0)
	{
		CurrentOptic = nullptr;
		return true;
	}
	Optics.Reset();
	//Optics.Add(nullptr);
	for (auto Mod : Mods)
	{
		if (ABaseWeaponScope* Optic = Cast<ABaseWeaponScope>(Mod.ActorModuleInfo))
		{
			Optics.Add(Optic);
		}
	}
	int32 Id = Optics.Find(CurrentOptic);
	if (Id == INDEX_NONE)
	{
		CurrentOptic = nullptr;
		return true;
	}
	Id += (Next ? 1 : -1);
	Id = (Id < 0 ? (Optics.Num() - 1) : Id);

	if (Optics.IsValidIndex(Id))
	{
		CurrentOptic = Optics[Id];
		return true;
	}
	if (Optics.IsValidIndex(0))
	{
		CurrentOptic = Optics[0];
		return true;
	}
	CurrentOptic = nullptr;
	return true;
}
#pragma endregion //AIM

#pragma region Modules

bool ABaseFireWeapon::AttachModule(UBaseWeaponModuleService* WeaponService, FWeaponSocketInfo Tag)
{
	if (!(IsValid(WeaponService) && WeaponService->InstalPlace && WeaponService->InstalPlace->GetItemOwner())) return false;
	if (!IsValid(GetWeaponService())) return false;
	FWeaponModuleInfo ModData;
	if (!WeaponService->GetModificationInfoItem(WeaponService->InstalPlace->GetItemOwner()->GetClass(), ModData)) return false;
	
	if (ModData.AsMesh)
	{

		FMeshComponentData ComponentData;
		ComponentData.MeshRef = ModData.Mesh;

		MeshManager->AddMesh(ComponentData);
	}
	else
	{
		if (!UKismetSystemLibrary::IsValidClass(ModData.Mod)) return false;

		auto Result = GetWorld()->SpawnActor(ModData.Mod);
		if (Result == nullptr) return false;
		if (ABaseItemActor* LItem = Cast<ABaseItemActor>(Result))
		{
			LItem->SetupItem(WeaponService->GetItemOwner(), false);
		}

		if (ABaseWeaponScope* Optic = Cast<ABaseWeaponScope>(Result))
		{
			CurrentOptic = Optic;
		}

		FName* SocketAttach = ModData.SocketAttach.Find(Tag.UID);
		FWeaponRepModData Data;
		Data.ActorModuleInfo = Result;
		Data.ObjectModuleInfo = WeaponService;
		Data.SocketAttach = (SocketAttach == nullptr ? FName(NAME_None) : *SocketAttach);

		Result->SetActorEnableCollision(false);
		Result->AttachToComponent(GetWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, (SocketAttach == nullptr ? FName(NAME_None) : *SocketAttach));
		Mods.Add(Data);
	}

	RefreshAttachment();

	if (ModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_FireSound)
	{
		CurrentFireSound = ModData.ModData.OnShotFireSound;
	}

	return true;
}

void ABaseFireWeapon::OnRep_AttachedModules()
{
	for (auto ModData : Mods)
	{
		if (!IsValid(ModData.ActorModuleInfo)) continue;
		ModData.ActorModuleInfo->SetActorEnableCollision(false);
		ModData.ActorModuleInfo->AttachToComponent(GetWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ModData.SocketAttach);
	}
	RefreshAttachment();
	RefreshRender();
}

void ABaseFireWeapon::RefreshRender()
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer) return;
	if (!GetFireWeaponData()) return;

	FVector Origin;
	FVector Extend;
	float sphereR = 0.f;
	FRotator LCacheRotation = GetWeaponMesh()->GetRelativeRotation();
	GetWeaponMesh()->SetWorldRotation(FRotator(0.f));
	UKismetSystemLibrary::GetComponentBounds(GetWeaponMesh(), Origin, Extend, sphereR);
	GetWeaponMesh()->SetRelativeRotation(LCacheRotation);
}

bool ABaseFireWeapon::DetachModule(UBaseWeaponModuleService* WeaponService)
{
	if (!(IsValid(WeaponService) && WeaponService->InstalPlace && WeaponService->InstalPlace->GetItemOwner())) return false;

	FWeaponModuleInfo ModData;
	if (!WeaponService->GetModificationInfoItem(WeaponService->InstalPlace->GetItemOwner()->GetClass(), ModData)) return false;

	if (ModData.AsMesh)
	{
		FMeshComponentData ComponentData;
		ComponentData.MeshRef = ModData.Mesh;

		MeshManager->RemoveMesh(ComponentData);
	}
	else
	{
		int32 Idx = INDEX_NONE;
		for (int32 LIdx = 0; LIdx < Mods.Num(); LIdx++)
		{
			if (Mods[LIdx].ObjectModuleInfo == WeaponService)
			{
				Idx = LIdx;
				break;
			}
		}
		if (Idx == INDEX_NONE) return false;

		AActor* Mod = Mods[Idx].ActorModuleInfo;
		if (!IsValid(Mod)) return false;

		CurrentOptic = (CurrentOptic == Mod ? nullptr : CurrentOptic);

		if (Mods[Idx].ActorModuleInfo)
		{
			Mods[Idx].ActorModuleInfo->Destroy();
		}
		Mods.RemoveAt(Idx);
	}

	if (ModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_FireSound)
	{
		if (GetFireWeaponData())
		{
			CurrentFireSound = GetFireWeaponData()->OnShotFireSound;
		}
	}

	RefreshAttachment();

	return true;
}

#pragma endregion

#pragma region NEXT_FIRETYPE


void ABaseFireWeapon::NextFireType()
{
	if (!GetFireWeaponData()) return;
	if (GetFireWeaponData()->FireTypes == 0) return;

	EWeaponFireType Last = CurrentFireType;
	const UEnum* LFireTypeEnum = StaticEnum<EWeaponFireType>();
	int32 IdxCurret = INDEX_NONE;
	for (int32 Idx = 0; Idx < LFireTypeEnum->NumEnums() - 1; Idx++)
	{
		EWeaponFireType WeaponType = static_cast<EWeaponFireType>(LFireTypeEnum->GetValueByIndex(Idx));
		if (CurrentFireType != WeaponType) continue;
		IdxCurret = Idx;
		break;
	}

	for (int32 Idx = (IdxCurret + 1); Idx < LFireTypeEnum->NumEnums() - 1; Idx++)
	{
		if (Idx >= LFireTypeEnum->NumEnums() - 1) break;
		
		EWeaponFireType WeaponType = static_cast<EWeaponFireType>(LFireTypeEnum->GetValueByIndex(Idx));
		if (!(GetFireWeaponData()->FireTypes & (uint8)WeaponType)) continue;

		CurrentFireType = WeaponType;
		break;

	}
	if (Last == CurrentFireType)
	{
		for (int32 Idx = 0; Idx < LFireTypeEnum->NumEnums() - 1; Idx++)
		{
			EWeaponFireType WeaponType = static_cast<EWeaponFireType>(LFireTypeEnum->GetValueByIndex(Idx));
			if (!(GetFireWeaponData()->FireTypes & (uint8)WeaponType)) continue;
			CurrentFireType = static_cast<EWeaponFireType>(LFireTypeEnum->GetValueByIndex(Idx));
			break;
		}
	}
	if (Last == CurrentFireType) return;

	OnChangeFireType.Broadcast(this);
	if (GetFireWeaponData() && GetFireWeaponData()->ChangeFireMode)
	{
		UGameplayStatics::SpawnSoundAttached(GetFireWeaponData()->ChangeFireMode, GetWeaponMesh(), FName());
	}

	if (!HasAuthority())
		Server_SetNextFireType(CurrentFireType);
}

void ABaseFireWeapon::Server_SetNextFireType_Implementation(EWeaponFireType FireType)
{
	CurrentFireType = FireType;
	OnChangeFireType.Broadcast(this);
}

#pragma endregion NEXT_FIRETYPE

#pragma region Tools

void ABaseFireWeapon::SetIsCharged(bool IsCharged)
{
	if (GetCurrentAmmo() < 1)
	{
		bIsCharged = false;
		return;
	}
	bIsCharged = IsCharged;
}

bool ABaseFireWeapon::IsReadyToUse()
{
	return CanFire();
}

bool ABaseFireWeapon::IsCharged()
{
	return bIsCharged;
}

void ABaseFireWeapon::RefreshAttachment()
{
	CurrentSettingControll = DefaultSettingControll;
	JoinTarget = FVector::ZeroVector;
	CurrentIK_Offset = FTransform();

	if (GetWeaponService())
	{
		CanBeFire = GetWeaponService()->HasModuleByTag(WeaponModificationType::Muzzle);
		if (!CanBeFire)
		{
			for (auto LService : GetWeaponService()->GetAttachedModules(false))
			{
				if (!IsValid(LService)) continue;
				if (LService->GetModificationTag() != WeaponModificationType::Muzzle) continue;
				CanBeFire = true;
				break;
			}
		}
		for (auto LService : GetWeaponService()->GetAttachedModules(false))
		{
			if (!(IsValid(LService) && IsValid(LService->InstalPlace) && LService->InstalPlace->GetItemOwner())) continue;

			FWeaponModuleInfo LModData;
			if (!LService->GetModificationInfoItem(LService->InstalPlace->GetItemOwner()->GetClass(), LModData)) return;

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_FireSound)
			{
				CurrentFireSound = LModData.ModData.OnShotFireSound;
			}

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_Spread)
			{
				CurrentSettingControll.SpreedCoef *= LModData.ModData.SpreadMultiplayer;
			}

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_RecoilH)
			{
				CurrentSettingControll.HorizontalRecoilCoefCam *= LModData.ModData.RecoilHMultiplayer;
				CurrentSettingControll.HorizontalRecoilCoefAnim *= LModData.ModData.AnimRecoilHMultiplayer;
			}

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_RecoilV)
			{
				CurrentSettingControll.VericalRecoilCoefCam *= LModData.ModData.RecoilVMultiplayer;
				CurrentSettingControll.VericalRecoilCoefAnim *= LModData.ModData.AnimRecoilVMultiplayer;
			}

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_IK_Offset)
			{
				CurrentIK_Offset = LModData.ModData.IK_Offset;
			}

			if (LModData.ModData.ActionMenu & (uint8)EWeaponModificationType::EWMT_OverrideJoinTarget)
			{
				JoinTarget = LModData.ModData.JoinTargetLocation;
			}

		}
	}

	OnChangeAmmo.Broadcast(this);
	RefreshRender();
}

UBaseWeaponService* ABaseFireWeapon::GetWeaponService()
{
	if (!IsValid(WeaponServiceCache))
	{
		WeaponServiceCache = (IsValid(GetItem()) ? GetItem()->GetServiceTyped<UBaseWeaponService>(UBaseWeaponService::StaticClass()) : nullptr);
	}
	return WeaponServiceCache;
}
bool ABaseFireWeapon::CanFire()
{
	if (GetCharacterOwner() && GetCharacterOwner()->GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		return true;
	}

	return (bIsCharged && CanBeFire && !bIsReload && CurrentFireType != EWeaponFireType::EWFT_None); // && HasModule(EWeaponAttachableItemType::EWAIT_Muzzle));
}

bool ABaseFireWeapon::NeedToReload()
{
	if (GetCharacterOwner() && GetCharacterOwner()->GetLocalRole() == ENetRole::ROLE_SimulatedProxy) return false;	

	return (GetCurrentAmmo() < 1);
}

void ABaseFireWeapon::ApplyRecoil()
{
	if (!GetFireWeaponData()) return;

	if (UKismetSystemLibrary::IsValidClass(GetRecoilData().RecoilCameraData.FireCameraShake))
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), GetRecoilData().RecoilCameraData.FireCameraShake, GetActorLocation(), 100.f, 200.f);
	}
	GetCharacterOwner()->AddControllerPitchInput(GetRecoilData().RecoilCameraData.VerticalRecoil * (-1.f) * GetRecoilData().RecoilCameraData.VerticalRecoilCoef * CurrentSettingControll.VericalRecoilCoefCam);
	GetCharacterOwner()->AddControllerYawInput(FMath::RandRange(GetRecoilData().RecoilCameraData.HorizontalRecoil.X, GetRecoilData().RecoilCameraData.HorizontalRecoil.Y) * CurrentSettingControll.HorizontalRecoilCoefCam);
}

FRotator ABaseFireWeapon::GetNextSpread()
{
	FRotator Result = FRotator(0.f);
	if (!GetFireWeaponData()) return Result;

	int32 Multiplayer = (NumShot > GetRecoilData().RecoilSpreadData.MaxSpreadOnShot ? GetRecoilData().RecoilSpreadData.MaxSpreadOnShot : NumShot);
	bool LISRight = (FMath::RandRange(0.f, 100.f) > 50.f);
	bool LIsUp = (FMath::RandRange(0.f, 100.f) > 50.f);

	Result.Yaw = FMath::RandRange(double(0.f), GetRecoilData().RecoilSpreadData.Spread.X);
	Result.Yaw = Result.Yaw * (LISRight ? 1.f : (-1.f));

	Result.Pitch = FMath::RandRange(double(0.f), GetRecoilData().RecoilSpreadData.Spread.Y);
	Result.Pitch = Result.Pitch * (LIsUp ? 1.f : (-1.f));

	if (GetRecoilData().RecoilSpreadData.SpreadPower)
	{
		Result = Result * GetRecoilData().RecoilSpreadData.SpreadPower->GetFloatValue(NumShot);
	}
	else
	{
		Result = Result * float((Multiplayer > 1 ? Multiplayer : 2.f) / 10.f);
	}

	Result = Result * CurrentSettingControll.SpreedCoef;


	return Result;
}

bool ABaseFireWeapon::SpawnProjectile(FRotator ProjectileRot)
{
	if (!GetFireWeaponData()) return false;
	if (!GetWeaponService()) return false;
	if (GetWeaponService()->GetSupportedAmmo().Num() < 1) return false;

	const FTransform SpawnTransform = GetWeaponMesh()->GetSocketTransform(GetFireWeaponData()->MuzzleSocket);

	if (!GetFireWeaponData()->bIsPellets)
	{
		auto ClassBulit = GetWeaponService()->GetCurrentBulit();
		if (ClassBulit == NULL) return false;

		ABaseBulit* LProjectile = GetWorld()->SpawnActorDeferred<ABaseBulit>(ClassBulit, SpawnTransform);
		if (!LProjectile) return false;

		LProjectile->SetOwner(GetOwner());
		LProjectile->FinishSpawning(SpawnTransform);
		LProjectile->Launch((UKismetMathLibrary::GetForwardVector(ProjectileRot) * GetFireWeaponData()->FireDistance), GetFireWeaponData()->FireDistance, Cast<ABaseBulit>(ClassBulit->GetDefaultObject())->DamageType);
	}
	else
	{
		auto ClassBulit = GetWeaponService()->GetCurrentBulit();
		if (ClassBulit == NULL) return false;
		float Angle = (GetFireWeaponData()->AnglePelletsSpread / 2.f);
		TArray<ABaseBulit*> Bulits;

		for (int32 LIdx = 0; LIdx < 10; LIdx++)
		{
			ABaseBulit* LProjectile = GetWorld()->SpawnActorDeferred<ABaseBulit>(ClassBulit, SpawnTransform);
			if (!LProjectile) return false;
			LProjectile->SetOwner(GetOwner());
			Bulits.Add(LProjectile);
		}

		for (auto CurrentBulet : Bulits)
		{
			for (auto IgnoreBulit : Bulits)
			{
				CurrentBulet->AddIgnoreActor(IgnoreBulit);
			}
			CurrentBulet->FinishSpawning(SpawnTransform);
			FRotator LDirection = FRotator(ProjectileRot.Pitch + FMath::FRandRange((-1.f * Angle), Angle), ProjectileRot.Yaw + FMath::FRandRange((-1.f * Angle), Angle), ProjectileRot.Roll + FMath::FRandRange((-1.f * Angle), Angle));
			CurrentBulet->Launch((UKismetMathLibrary::GetForwardVector(LDirection) * GetFireWeaponData()->FireDistance), GetFireWeaponData()->FireDistance, Cast<ABaseBulit>(ClassBulit->GetDefaultObject())->DamageType);
		}
	}
	return true;
}

void ABaseFireWeapon::Multicast_ChangeFireState_Implementation(bool State)
{
	if (!GetCharacterOwner()) return;
	if (HasAuthority() || (GetCharacterOwner() && GetCharacterOwner()->IsLocallyControlled())) return;
	Fire(State);
}

bool ABaseFireWeapon::Shooting()
{
	if (!CanFire()) return false;
	if (!GetFireWeaponData()) return false;
	if (!GetCharacterOwner()) return false;

	if (NeedToReload())
	{
		OnWeaponFireStatus.Broadcast(this, false);

		UGameplayStatics::SpawnSoundAttached((GetFireWeaponData()->EmptySound), GetWeaponMesh(), ""); //Next Call Fire, will be call the "Reload"
		GetWorld()->GetTimerManager().ClearTimer(Timer_AutoFire);
		NumShot = 0;
		bIsFire = false;
		return false;
	}


	FTransform LTarget = GetWeaponMesh()->GetSocketTransform(GetFireWeaponData()->MuzzleSocket);
	LTarget.SetRotation(FQuat(LTarget.Rotator() + GetNextSpread()));
	FRotator ProjectileRot = LTarget.Rotator();

	if (/*/HasAuthority() &&*/ !SpawnProjectile(ProjectileRot)) return false;

	if (IsLocallyControlled() || HasAuthority())
	{
		SubstructAmmo(1);
		NumShot++;
		ApplyRecoil();
	}

	UGameplayStatics::SpawnSoundAttached(CurrentFireSound, GetWeaponMesh(), GetFireWeaponData()->MuzzleSocket);

	if (GetFireWeaponData()->FireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(GetFireWeaponData()->FireEffect, GetWeaponMesh(), GetFireWeaponData()->MuzzleSocket);
	}

	if (GetFireWeaponData()->AmmoExit)
	{
		UGameplayStatics::SpawnEmitterAttached(GetFireWeaponData()->AmmoExit, GetWeaponMesh(), GetFireWeaponData()->SocketAmmoExit);
	}

	if (GetCharacterOwner() && GetCharacterOwner()->GetMesh() && GetCharacterOwner()->GetMesh()->GetAnimInstance())
	{
		GetCharacterOwner()->GetMesh()->GetAnimInstance()->Montage_Play(GetFireWeaponData()->CharacterFireAnimated);
	}

	if (GetWeaponMesh() && GetWeaponMesh()->GetAnimInstance())
	{
		GetWeaponMesh()->GetAnimInstance()->Montage_Play(GetFireWeaponData()->WeaponFireAnimated);
	}

	MakeNoise(1.f, Cast<APawn>(GetOwner()), GetActorLocation());

	OnChangeAmmo.Broadcast(this);
	OnWeaponFire.Broadcast(this);
	return true;
}

UBaseFireWeaponData* ABaseFireWeapon::GetFireWeaponData()
{
	if (IsValid(CachedCastWeaponData)) return CachedCastWeaponData;
	
	CachedCastWeaponData = Cast<UBaseFireWeaponData>(WeaponData);	
	return CachedCastWeaponData;
}

FRecoilData ABaseFireWeapon::GetRecoilData()
{
	if (!GetFireWeaponData()) return FRecoilData();
	return GetFireWeaponData()->RecoilSettings;
}

UCurveVector* ABaseFireWeapon::GetMovementSwayCurve()
{
	if (!GetFireWeaponData()) return nullptr;
	return GetFireWeaponData()->MovementSwayCurve;
}

bool ABaseFireWeapon::UseStatsForMovementSway()
{
	if (!GetFireWeaponData()) return false;
	return GetFireWeaponData()->ControlMovementSwayByStats;
}

void ABaseFireWeapon::OnRep_WeaponData()
{
	Super::OnRep_WeaponData();
	if (!GetFireWeaponData()) return;
	if (!GetWeaponMesh()) return;
	GetWeaponMesh()->SetAnimInstanceClass(GetFireWeaponData()->WeaponAnimClass);
	if (UBaseWeaponAnimationInstance* AnimInst = Cast<UBaseWeaponAnimationInstance>(GetWeaponMesh()->GetAnimInstance()))
	{
		AnimInst->SetWeaponOwner(this);
	}
	RefreshRender();
}
UAnimInstance* ABaseFireWeapon::GetCharacterAnimInstance()
{
	if (!(GetCharacterOwner() && GetCharacterOwner()->GetMesh() && GetCharacterOwner()->GetMesh()->GetAnimInstance())) return nullptr;
	return GetCharacterOwner()->GetMesh()->GetAnimInstance();
}
UAnimInstance* ABaseFireWeapon::GetAnimationInstace()
{
	if (!(GetWeaponMesh() && GetWeaponMesh()->GetAnimInstance())) return nullptr;
	return GetWeaponMesh()->GetAnimInstance();
}

void ABaseFireWeapon::MagazineAnimation(FName PlayerSocket, FTransform Offset, bool ToPlayer)
{
	for (auto Module : Mods)
	{
		if (!(IsValid(Module.ObjectModuleInfo) && IsValid(Module.ActorModuleInfo))) continue;

		if (Module.ObjectModuleInfo->GetModificationTag() != WeaponModificationType::Magazine) continue;

		if (ToPlayer)
		{
			Module.ActorModuleInfo->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			Module.ActorModuleInfo->AttachToComponent(GetCharacterOwner()->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, PlayerSocket);
			Module.ActorModuleInfo->SetActorRelativeTransform(Offset);
		}
		else
		{
			FWeaponModuleInfo ModData;
			if (!Module.ObjectModuleInfo->GetModificationInfoItem(GetItem()->GetClass(), ModData)) return;
			Module.ActorModuleInfo->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			Module.ActorModuleInfo->AttachToComponent(
				GetWeaponMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				Module.SocketAttach);
		}

	}
}

int32 ABaseFireWeapon::GetMaxAmmo()
{
	if (GetFireWeaponData())
	{
		return GetFireWeaponData()->MaxAmmo;
	}
	return 0;
}

int32 ABaseFireWeapon::GetCurrentAmmo()
{
	if (IsValid(GetWeaponService()))
	{
		return GetWeaponService()->GetCurrentAmmo();
	}
	return 0;
}

void ABaseFireWeapon::SubstructAmmo(int32 Amount)
{
	if(IsValid(GetWeaponService()))
	{
		GetWeaponService()->SubstructAmmo(Amount);
	}
}




#pragma endregion

#pragma region ANIM

FTransform ABaseFireWeapon::GetLeftHandIK()
{
	if (!GetWeaponMesh()) return FTransform();
	if (GetWeaponMesh()->MeshObject == nullptr) return FTransform();

	FTransform IkResult = GetWeaponMesh()->GetSocketTransform(FName("SKT_LeftHand_IK"));
	FTransform Offset = (GetFireWeaponData()->UseOffsetDebug ? GetFireWeaponData()->DebugIKOffset : CurrentIK_Offset);

	return Offset.GetRelativeTransformReverse(IkResult);
}

FTransform ABaseFireWeapon::GetBasePoseOffset(bool ForFPS)
{
	if (GetFireWeaponData())
	{
		return (ForFPS ? GetFireWeaponData()->DefaultPoseOffset : GetFireWeaponData()->DefaultPoseOffsetTPS);
	}
	return FTransform();
}

FTransform ABaseFireWeapon::GetAIMPoseOffset(bool ForFPS)
{
	if (GetFireWeaponData())
	{
		return (ForFPS ? GetFireWeaponData()->AIM_PoseOffset : GetFireWeaponData()->AIM_PoseOffsetTPS);
	}
	return FTransform();
}

FTransform ABaseFireWeapon::GetMovePoseOffset()
{
	if (GetFireWeaponData())
	{
		return GetFireWeaponData()->MovePoseOffset;
	}
	return FTransform();
}

FTransform ABaseFireWeapon::GetHighPortPose()
{
	if (GetFireWeaponData())
	{
		return GetFireWeaponData()->HighPortPose;
	}
	return FTransform();
}

FFirearmStats ABaseFireWeapon::GetFirearmStats()
{
	if (!GetFireWeaponData()) return FFirearmStats();
	return GetFireWeaponData()->FirearmStats;
}

FTransform ABaseFireWeapon::GetSightSocketTransform()
{
	if (IsAiming && IsValid(CurrentOptic) && IsValid(CurrentOptic->GetMesh()) && IsValid(CurrentOptic->GetMesh()->GetStaticMesh()))
	{
		return CurrentOptic->GetMesh()->GetSocketTransform(FName("SKT_Sight"));
	}
	if (GetWeaponMesh()->GetSkeletalMeshAsset() != nullptr)
	{
		return GetWeaponMesh()->GetSocketTransform(FName("SKT_Sight"));
	}
	return FTransform();
}

bool ABaseFireWeapon::IsUseIK()
{
	if (GetFireWeaponData())
	{
		return GetFireWeaponData()->bIsUseIK;
	}
	return true;
}

FVector ABaseFireWeapon::GetJoinTargetLocation()
{
	if (GetFireWeaponData()->UseJoinDebug)
	{
		return GetFireWeaponData()->DebugJoin;
	}
	if (!JoinTarget.IsNearlyZero()) return JoinTarget;
	
	if (GetFireWeaponData())
	{
		return GetFireWeaponData()->JoinTargetLocation;
	}
	return FVector(20.0f, 0.f, 0.f);
}

#pragma endregion ANIM
