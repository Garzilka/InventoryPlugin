// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponService.h"

#include "BaseWeaponModuleService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BasePickupService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseQualityService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Bulit/BaseBulit.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseFireWeapon.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseWeapon.h"

#pragma region ENGINE

UBaseWeaponService::UBaseWeaponService()
{
}

void UBaseWeaponService::BeginPlay(UBaseItem* Item)
{
	Super::BeginPlay(Item);

	if (!GetItemOwner()) return;	

	if (UBaseHandItemService* BHIS = GetItemOwner()->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass()))
	{
		BHIS->OnHandItemObjectPlayAnimation.AddUniqueDynamic(this, &UBaseWeaponService::PlayEquipAnimation);
	}
	
	if (!GetItemOwner()->GetWorldPlacementService()) return;	
	GetItemOwner()->GetWorldPlacementService()->OnPreSpawnItem.AddUniqueDynamic(this, &UBaseWeaponService::Event_PreSpawnItem);	
}

void UBaseWeaponService::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseWeaponService, ModificationData);
}


void UBaseWeaponService::OnRep_AttachedModules()
{
	if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(GetSpawnedRef()))
	{
		WeaponRef->RefreshAttachment();
	}
	OnWeaponServiceUpdate.Broadcast(this);
}

#pragma endregion //ENGINE

#pragma region ATTACH
bool UBaseWeaponService::AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate)
{
	if (!IsValid(Item)) return false;

	UBaseWeaponModuleService* Service = Item->GetServiceTyped<UBaseWeaponModuleService>(UBaseWeaponModuleService::StaticClass());
	if (!IsValid(Service)) return false;
	
	int32 Idx = ModificationData.Find(TargetPlacement);
	if  (Idx == INDEX_NONE) return false;

	FWeaponModuleInfo ModData;
	if (!Service->GetModificationInfoItem(GetItemOwner()->GetClass(), ModData)) return false;
	
	if (!Item->HasAuthority())
	{
		Server_AttachItem(Item, TargetPlacement, NeedCreate);
		return true;
	}

	for (auto SocketUID : Service->GetBlockSlotBySocketUID())
	{
		AddBlockSocket(SocketUID);
	}

	Service->InstalTo = TargetPlacement;
	Service->WeaponOwner = this;
	Service->InstalPlace = this;
	Service->OnWeaponModuleServiceUpdate.AddUniqueDynamic(this, &UBaseWeaponService::Event_UpdateModule);

	if (auto Module = ModificationData[Idx].AttachedModule)
	{
		if (Module->GetItemOwner() && Module->GetItemOwner()->GetPickupService())
		{
			Module->GetItemOwner()->GetPickupService()->PickupItem(GetOwningPlayerPawn());
		}
	}
	Item->SwitchParrent(this);
	ModificationData[Idx].AttachedModule = Service;
	if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(GetSpawnedRef()))
	{
		WeaponRef->AttachModule(Service, TargetPlacement);
	}

	OnRep_AttachedModules();
	return true;
}

void UBaseWeaponService::AddBlockSocket(FName SocketUID)
{
	if (SocketsBlock.Contains(SocketUID)) return;
	
	for (auto& SocketData : ModificationData)
	{
		if (!SocketData.AttachedModule) continue;			
		if (SocketData.UID != SocketUID) continue;
			
		DetachItem(SocketData.AttachedModule->GetItemOwner());
		break;
	}
	SocketsBlock.AddUnique(SocketUID);
	OnRep_AttachedModules();
}

void UBaseWeaponService::RemoveBlockSocket(FName SocketUID)
{
	SocketsBlock.Remove(SocketUID);
	OnRep_AttachedModules();
}

void UBaseWeaponService::Server_AttachItem_Implementation(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate)
{
	if (!Item->HasAuthority()) return;

	AttachItem(Item, TargetPlacement, NeedCreate);
}

#pragma endregion //ATTACH

#pragma region DETACH

bool UBaseWeaponService::DetachItem(UBaseItem* Module)
{
	if (!IsValid(Module)) return false;
	UBaseWeaponModuleService* Service = Module->GetServiceTyped<UBaseWeaponModuleService>(UBaseWeaponModuleService::StaticClass());

	int32 Idx = INDEX_NONE;
	for (int32 LIdx = 0; LIdx < ModificationData.Num(); LIdx++)
	{
		if (!(ModificationData[LIdx].AttachedModule && ModificationData[LIdx].AttachedModule->GetItemOwner())) continue;
		
		if (ModificationData[LIdx].AttachedModule->GetItemOwner() != Module) continue;
		Idx = LIdx;
		break;
	}
	if  (Idx == INDEX_NONE) return true;

	if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(GetSpawnedRef()))
	{
		WeaponRef->DetachModule(Service);
	}

	for (auto SocketUID : Service->GetBlockSlotBySocketUID())
	{
		RemoveBlockSocket(SocketUID);
	}
	
	if(Service->OnWeaponModuleServiceUpdate.IsAlreadyBound(this, &UBaseWeaponService::Event_UpdateModule))
		Service->OnWeaponModuleServiceUpdate.RemoveDynamic(this, &UBaseWeaponService::Event_UpdateModule);
	
	auto LItem = ModificationData[Idx].AttachedModule->GetItemOwner();
	ModificationData[Idx].AttachedModule = nullptr;
	if (LItem && LItem->GetPickupService())
	{
		Service->DetachAllAttaches();
		LItem->GetPickupService()->PickupItem(GetOwningPlayerPawn());
	}

	OnWeaponServiceUpdate.Broadcast(this);

	return true;
}

#pragma endregion //DETACH

#pragma region SAVE/LOAD

void UBaseWeaponService::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }

	if (Ar.IsSaving())
	{
		SaveItemData.Reset();
		for (auto Module : ModificationData)
		{
			if (!(IsValid(Module.AttachedModule) && IsValid(Module.AttachedModule->GetItemOwner()))) continue;
			FWeaponServiceSave NewData;
			NewData.SaveItem = Module.AttachedModule->GetItemOwner()->GetClass();
			NewData.SocketUID = Module.AttachedModule->InstalTo.UID;
			SaveItemData.Add(NewData);
		}
		Super::Serialize(Ar);

		for (auto Module : ModificationData)
		{
			if (!(IsValid(Module.AttachedModule) && IsValid(Module.AttachedModule->GetItemOwner()))) continue;
			Module.AttachedModule->GetItemOwner()->Serialize(Ar);
		}

		return;
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		for (auto SaveData : SaveItemData)
		{
			FInventoryItemData ItemData;
			ItemData.Amount = 1;
			ItemData.ItemClass = SaveData.SaveItem;
			UBaseItem* Item = UInventoryLibrary::MakeItem(GetOwner(), ItemData);
			if (!Item) continue;
			for (auto& Module : ModificationData)
			{
				if (Module.UID != SaveData.SocketUID) continue;
				AttachItem(Item, Module);
				break;
			}
			Item->Serialize(Ar);
		}
	}
}

#pragma endregion //SAVE/LOAD

#pragma region WEAPON_FIRE

void UBaseWeaponService::Event_WeaponFire(ABaseFireWeapon* Weapon)
{
	if (GetSpawnedRef() != Weapon) return;
	if(!(GetItemOwner() && GetItemOwner()->GetQualityService())) return;

	GetItemOwner()->GetQualityService()->ApplyDamage(DamagePerFire);
}

#pragma endregion WEAPON_FIRE

#pragma region TOOLS

void UBaseWeaponService::Event_UpdateModule(UBaseWeaponModuleService* ModuleRef)
{
	OnRep_AttachedModules();
}

TSubclassOf<ABaseBulit> UBaseWeaponService::GetCurrentBulit()
{
	if (CurrentBulit != TSubclassOf<ABaseBulit>()) return CurrentBulit;
	if (CurrentAmmo == TSubclassOf<UBaseItem>()) return TSubclassOf<ABaseBulit>();

	TSubclassOf<ABaseBulit>* Result = SupportedAmmo.Find(CurrentAmmo);
	CurrentBulit = (Result == nullptr ? TSubclassOf<ABaseBulit>() : *Result);
	return CurrentBulit;
}

TArray<UBaseWeaponModuleService*> UBaseWeaponService::GetAttachedModules(bool OnlySelf)
{
	TArray<UBaseWeaponModuleService*> Result;
	if(OnlySelf)
	{
		for (auto& Module : ModificationData)
		{
			if (!Module.AttachedModule) continue;
			Result.Add(Module.AttachedModule);
		}
		return Result;
	}

	for (auto& Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		Result.Add(Module.AttachedModule);
		Result.Append(Module.AttachedModule->GetAttachedModules(OnlySelf));
	}
	return Result;
}

UBaseItem* UBaseWeaponService::GetModificationBySocket(FWeaponSocketInfo SocketInfo)
{
	int32 Idx = ModificationData.Find(SocketInfo);
	if (Idx == INDEX_NONE) return nullptr;
	return (IsValid(ModificationData[Idx].AttachedModule) ? ModificationData[Idx].AttachedModule->GetItemOwner() : nullptr);
}

TArray<UBaseItem*> UBaseWeaponService::GetModulesByTag(FGameplayTag Tag)
{
	TArray<UBaseItem*> Result;
	for (auto Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		if (!(Tag == Module.AttachTag || Tag == Module.AttachedModule->InstalTo.AttachTag)) continue;
		Result.Add(Module.AttachedModule->GetItemOwner());
	}
	return Result;
}

bool UBaseWeaponService::HasModuleByTag(FGameplayTag Tag)
{
	for (auto Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		if (!(Tag == Module.AttachTag || Tag == Module.AttachedModule->InstalTo.AttachTag)) continue;
		return true;
	}
	return false;
}


ABaseItemActor* UBaseWeaponService::GetSpawnedRef()
{
	if (!GetItemOwner()) return nullptr;
	return (IsValid(GetItemOwner()->GetWorldPlacementService()) ? GetItemOwner()->GetWorldPlacementService()->GetSpawnedRef() : nullptr);
}

TArray<UBaseItem*> UBaseWeaponService::GetAttachedItem()
{
	TArray<UBaseItem*> Result;
	for (auto& Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		Result.Add(Module.AttachedModule->GetItemOwner());
		Result.Append(Module.AttachedModule->GetAttachedItem());
	}
	return Result;
}

TArray<UObject*> UBaseWeaponService::GetAllChieldObject()
{
	TArray<UObject*> Result;
	for (auto& Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		Result.Add(Module.AttachedModule->GetItemOwner());
		Result.Append(Module.AttachedModule->GetAttachedItem());
	}
	return Result;
}

void UBaseWeaponService::Event_PreSpawnItem(UBaseWorldItemService* WorldService, ABaseItemActor* SpawnedReference)
{
    if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(SpawnedReference))
    {
        Weapon->InitWeapon(WeaponData);
        for (auto Module : GetAttachedModules(true))
        {
            Weapon->AttachModule(Module, Module->InstalTo);
            Module->WeaponOwner = this;
            Module->RefreshAttachment();
        }
    }
    if (DamagePerFire > 0.f && GetItemOwner() && GetItemOwner()->GetQualityService())
    {
        if (ABaseFireWeapon* FireWeapon = Cast<ABaseFireWeapon>(SpawnedReference))
        {
            FireWeapon->OnWeaponFire.AddUniqueDynamic(this, &UBaseWeaponService::Event_WeaponFire);
        }
    }
}


void UBaseWeaponService::SubstructAmmo(int32 Amount)
{
	CurrentAmountAmmo -= Amount;
	if (CurrentAmountAmmo < 0)
		CurrentAmountAmmo = 0;
}

void UBaseWeaponService::PlayEquipAnimation(UBaseHandItemService* CharacterInstigator, bool IsEquip)
{
	if (!GetItemOwner()) return;
	if (!GetItemOwner()->GetWorldPlacementService()) return;
	if (ABaseWeapon* Weapon = Cast<ABaseWeapon>(GetItemOwner()->GetWorldPlacementService()->GetSpawnedRef()))
	{
		UAnimMontage* Animation = (IsEquip ? EquipAnimation : UnequipAnimation);
		if (Animation && Weapon->GetAnimationInstace())
		{
			Weapon->GetAnimationInstace()->Montage_Play(Animation);
		}
	}
}
#pragma endregion //TOOLS
