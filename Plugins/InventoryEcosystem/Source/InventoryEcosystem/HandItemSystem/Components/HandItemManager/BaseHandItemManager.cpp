

#include "BaseHandItemManager.h"

#include "TimerManager.h"
#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseFireWeapon.h"
#include "InventoryEcosystem/HandItemSystem/Interface/InventoryActorControll.h"
#include "InventoryEcosystem/HandItemSystem/Library/HandItemLibrary.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ItemServices/BaseHandItemService.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/MemoryReader.h"
#include "Animation/AnimInstance.h"


#pragma region ENGINE

UBaseHandItemManager::UBaseHandItemManager()
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UBaseHandItemManager::BeginPlay()
{
	Super::BeginPlay();
	
	LoadData();
}

void UBaseHandItemManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBaseHandItemManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseHandItemManager, bIsAIM);
	DOREPLIFETIME(UBaseHandItemManager, ItemData);
	DOREPLIFETIME(UBaseHandItemManager, HandItemState);
	
}


bool UBaseHandItemManager::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	return WroteSomething;
}

void UBaseHandItemManager::LoadData()
{
	if (!HasAuthority()) return;
	
	if (IsLoaded) return;

	if (!(HasBegunPlay() && IsSerialized && GetMesh())) return;
	
	if (HasAuthority() && !SavedData.IsValidData())
	{
		for (int32 Idx = 0; Idx < ItemData.Slots.Num(); Idx++)
		{
			SetupItem(UInventoryLibrary::MakeItem(GetOwner(), ItemData.Slots[Idx].ItemSetting), Idx);
		}
		IsLoaded = true;
		return;
	}
	
	ItemData.Slots.Reset();
	int32 Idx = 0;
	for (auto LItemData : SavedData.SavedData)
	{
		FHandSlotSetting NewSlot;
		NewSlot.SlotType = LItemData.SlotTag;
		ItemData.Slots.Add(NewSlot);
	
		UBaseItem* Item = UInventoryLibrary::MakeItem(GetOwner(), FInventoryItemData(LItemData.SavedClass));
		if (Item)
		{
			FMemoryReader MemoryReader(LItemData.ItemData, true);
			FSaveGameArchive ArLoad(MemoryReader);
			Item->BeginPlay();
			Item->Serialize(ArLoad);
			SetupItem(Item, Idx);
		}
		Idx++;
	}
	
	IsLoaded = true;
	OnSlotsUpdate.Broadcast(this);
}

void UBaseHandItemManager::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }
	
	if (Ar.IsSaving())
	{
		SavedData = FHandItemManagerSave(this);
	
		Super::Serialize(Ar);
		Ar << SavedData;
		for (auto LItemData : GetItemData())
		{
			if (IsValid(LItemData.ActorRef) && IsValid(LItemData.ActorRef->GetItem()))
			{
				LItemData.ActorRef->GetItem()->Serialize(Ar);
			}
		}
		return;
	}
	
	Super::Serialize(Ar);
	Ar << SavedData;
	IsSerialized = true;
	LoadData();
}
#pragma endregion ENGINE


#pragma region USER_CONTROLL

void UBaseHandItemManager::FirstAction(bool bUse)
{
	if (!IsValid(GetCurrentActor())) return;

	IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
	if (LControllInterface == nullptr) return;

	LControllInterface->Execute_I_FirstAction(GetCurrentActor(), bUse);
}

void UBaseHandItemManager::SecondAction(bool bUse)
{
	if (IsValid(GetCurrentActor()))
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_InterpolateFovTo))
			GetWorld()->GetTimerManager().ClearTimer(Timer_InterpolateFovTo);

		GetWorld()->GetTimerManager().SetTimer(Timer_InterpolateFovFrom, this, &UBaseHandItemManager::InterpolateFOVFrom, 0.001f, true);
	}

	IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
	if (LControllInterface == nullptr) return;

	LControllInterface->Execute_I_SecondAction(GetCurrentActor(), bUse);

	if (!GetOwningPlayerPawn()) return;
	bIsAIM = bUse;
	if (!HasAuthority())
	{
		Server_IsAIM(bUse);
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!IsValid(PlayerController) || (IsValid(PlayerController) && !IsValid(PlayerController->PlayerCameraManager))) return;

	PreviewTimeFOV = GetWorld()->GetTimeSeconds();

	if (bUse)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_InterpolateFovFrom))
			GetWorld()->GetTimerManager().ClearTimer(Timer_InterpolateFovFrom);

		GetWorld()->GetTimerManager().SetTimer(Timer_InterpolateFovTo, this, &UBaseHandItemManager::InterpolateFOVTo, 0.001f, true);
	}
	else
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_InterpolateFovTo))
			GetWorld()->GetTimerManager().ClearTimer(Timer_InterpolateFovTo);

		GetWorld()->GetTimerManager().SetTimer(Timer_InterpolateFovFrom, this, &UBaseHandItemManager::InterpolateFOVFrom, 0.001f, true);
	}
}

void UBaseHandItemManager::MeleeAction()
{
	TriggerInput(FGameplayTag());
}

void UBaseHandItemManager::ReloadAction()
{
	if (!IsValid(GetCurrentActor())) return;

	IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
	if (LControllInterface == nullptr) return;

	if (LControllInterface->Execute_I_ReloadAction(GetCurrentActor(), true))
	{
		FirstAction(false);
		SecondAction(false);
	}
}

#pragma endregion //UserControll

#pragma region EquipItem

bool UBaseHandItemManager::EquipItem(int32 Slot)
{
	if (!(!IsPlayEquipMontagne && IsValid(GetMesh()))) return false;

	FirstAction(false);
	SecondAction(false);
	
	if (!(HasAuthority() || IsLocallyControlled())) return false;

	int32 TargetSlot = (Slot == GetSlotIdByItem(HandItemState.TargetItem) ? INDEX_NONE : Slot);
	UBaseItem* NewTargetItem = (ItemData.Slots.IsValidIndex(TargetSlot) ? ItemData.Slots[TargetSlot].ItemRef : nullptr);
	
	
	if (!HasAuthority())
	{
		Server_EquipItem(Slot);
	}

	if (GetCurrentActor() && GetCurrentActor()->GetItem() != NewTargetItem)
	{
		if (HasAuthority())
		{
			if (GetCurrentActor()->GetItem())
			{
				if (UBaseHandItemService* HandItemService = GetCurrentActor()->GetItem()->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass()))
				{
					HandItemService->DoUnequipLogic(GetOwningPlayerPawn());
				}
			}
			IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
			if (LControllInterface == nullptr) return false;
			LControllInterface->Execute_I_UnequipActor(GetCurrentActor());
		}
		HandItemState.TargetItem = NewTargetItem;
		OnRep_State();
		OnNewCurrentSlot.Broadcast(this, nullptr, TargetSlot);
		return true;
	}
	
	if (!IsValid(GetCurrentActor()) && IsValid(NewTargetItem))
	{
		UBaseWorldItemService* WorldService = (IsValid(NewTargetItem) ? NewTargetItem->GetWorldPlacementService() : nullptr);
		if (HasAuthority() && WorldService)
		{
			if (UBaseHandItemService* HandItemService = NewTargetItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass()))
			{
				HandItemService->DoEquipLogic(GetOwningPlayerPawn());
			}
			IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(WorldService->GetSpawnedRef());
			if (LControllInterface == nullptr) return false;
			LControllInterface->Execute_I_EquipActor(WorldService->GetSpawnedRef());
		}
		HandItemState.TargetItem = NewTargetItem;
		OnRep_State();
		OnNewCurrentSlot.Broadcast(this, (WorldService ? WorldService->GetSpawnedRef() : nullptr), TargetSlot);
		return true;
	}

	return false;
}

void UBaseHandItemManager::Server_EquipItem_Implementation(int32 InTargetSlot)
{
	EquipItem(InTargetSlot);
}

void UBaseHandItemManager::OnRep_State()
{
	if (!IsValid(HandItemState.CurrentItem) && !IsValid(HandItemState.TargetItem))
	{
		OnNewCurrentSlot.Broadcast(this, nullptr, INDEX_NONE);
		IsPlayEquipMontagne = false;
		return;
	}

	if (HandItemState.CurrentItem == nullptr)
	{
		if (HandItemState.TargetItem == nullptr) return;

		UBaseHandItemService* HandItemService = HandItemState.TargetItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
		FEquipAnimation Result;
		int32 Idx = GetSlotIdByItem(HandItemState.TargetItem);
		if (!(HandItemService && HandItemState.TargetItem->GetWorldPlacementService() && HandItemService->GetHandData(ItemData.Slots[Idx].SlotType, Result))) return;

		auto Animation = Result.EquipAnimation;
		if (Animation)
		{
			IsPlayEquipMontagne = true;
			HandItemService->PlayEquipAnimation(Result, true);
			GetMesh()->GetAnimInstance()->Montage_Play(Animation);
		}
		else
		{
			NotifyEquip(EEquipNotifyType::EENT_Equip);
		}

		if (HasAuthority())
		{
			if (IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(HandItemState.TargetItem->GetWorldPlacementService()->GetSpawnedRef()))
			{
				LControllInterface->Execute_I_EquipActor(HandItemState.TargetItem->GetWorldPlacementService()->GetSpawnedRef());
			}
		}
		return;
	}

	if (HandItemState.TargetItem == nullptr)
	{
		if (HandItemState.CurrentItem == nullptr) return;
		UBaseHandItemService* HandItemService = HandItemState.CurrentItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
		FEquipAnimation Result;
		int32 Idx = GetSlotIdByItem(HandItemState.CurrentItem);
		if (!(HandItemService && HandItemService->GetHandData(ItemData.Slots[Idx].SlotType, Result))) return;
		auto Animation = Result.UnequipAnimation;
		if (Animation)
		{
			IsPlayEquipMontagne = true;
			HandItemService->PlayEquipAnimation(Result, false);
			GetMesh()->GetAnimInstance()->Montage_Play(Animation);
		}
		else
		{
			NotifyEquip(EEquipNotifyType::EENT_Unequip);
		}
	}

	if (HandItemState.CurrentItem != nullptr && HandItemState.TargetItem != nullptr)
	{
		UBaseHandItemService* HandItemService = HandItemState.CurrentItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
		FEquipAnimation Result;
		int32 Idx = GetSlotIdByItem(HandItemState.CurrentItem);
		if (Idx == INDEX_NONE) return;
		if (!(HandItemService && HandItemService->GetHandData(ItemData.Slots[Idx].SlotType, Result))) return;
		auto Animation = Result.UnequipAnimation;
		if (Animation)
		{
			IsPlayEquipMontagne = true;
			HandItemService->PlayEquipAnimation(Result, false);
			GetMesh()->GetAnimInstance()->Montage_Play(Animation);
		}
		else
		{
			NotifyEquip(EEquipNotifyType::EENT_Unequip);
		}
		return;
	}
}

void UBaseHandItemManager::NetMulticast_EquipItem_Implementation(UAnimMontage* Animation)
{
	if (IsLocallyControlled()) return;
	if (HasAuthority()) return;

	GetMesh()->GetAnimInstance()->Montage_Play(Animation);
}

bool UBaseHandItemManager::UnequipCurrentActor()
{
	return EquipItem(INDEX_NONE);
}

void UBaseHandItemManager::NotifyEquip(EEquipNotifyType NotifyType)
{
	IsPlayEquipMontagne = false;
	if (NotifyType == EEquipNotifyType::EENT_Unequip)
	{
		if (GetCurrentActor())
		{
			int32 Idx = GetSlotIdByItem(HandItemState.CurrentItem);
			UBaseHandItemService* HandItemService = HandItemState.CurrentItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
			FEquipAnimation Result;
			if (!(HandItemService && HandItemService->GetHandData(ItemData.Slots[Idx].SlotType, Result))) return;

			GetCurrentActor()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Result.UnequipSocketAttach);
			GetCurrentActor()->SetActorRelativeTransform(Result.UnequipSocketOffset);
		}
		HandItemState.CurrentItem = nullptr;
		OnRep_State();
		return;
	}

	if (NotifyType == EEquipNotifyType::EENT_Equip)
	{
		HandItemState.CurrentItem = HandItemState.TargetItem;
		if (HandItemState.CurrentItem == nullptr) return;

		int32 Idx = GetSlotIdByItem(HandItemState.CurrentItem);
		UBaseHandItemService* HandItemService = HandItemState.TargetItem->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
		FEquipAnimation Result;
		if (!(HandItemService && HandItemService->GetHandData(ItemData.Slots[Idx].SlotType, Result))) return;

		GetCurrentActor()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Result.EquipSocketAttach);
		GetCurrentActor()->SetActorRelativeTransform(Result.EquipSocketOffset);
		OnNewCurrentSlot.Broadcast(this, GetCurrentActor(), GetSlotIdByItemActor(GetCurrentActor()));
	}
}
#pragma endregion //EquipItem

#pragma region SetupItem

bool UBaseHandItemManager::SetupItem(UBaseItem* InTarget, int32 Slot)
{
	if (!(IsValid(InTarget) && IsValid(InTarget->GetWorldPlacementService()) && ItemData.Slots.IsValidIndex(Slot))) return false;

	UBaseHandItemService* HandItemService = InTarget->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass());
	if (!IsValid(HandItemService)) return false;
	FEquipAnimation Animation;
	if (!HandItemService->GetHandData(ItemData.Slots[Slot].SlotType, Animation)) return false;

	if (!HasAuthority())
	{
		Server_SetupItem(InTarget, Slot);
		return true;
	}
	if (!InTarget->SwitchParrent(this))
	{
		return false;
	}
	InTarget->ChangeOwner(GetOwner());

	int32 Idx = GetSlotIdByItem(InTarget);
	if (Idx != INDEX_NONE)
	{
		RemoveItemByItem(InTarget);
	}

	ItemData.Slots[Slot].ItemRef = InTarget;
	ItemData.Slots[Slot].ActorRef = InTarget->GetWorldPlacementService()->SpawnItem(FSpawnWorldItemServiceContext(GetOwner()->GetActorTransform()));
	if (!ItemData.Slots[Slot].ActorRef) return false;

	ItemData.Slots[Slot].ActorRef->SetOwner(GetOwner());
	if (ItemData.Slots[Slot].ActorRef && GetSlotIdByItem(HandItemState.CurrentItem) != Slot)
	{
		FEquipAnimation Result;
		if (HandItemService && HandItemService->GetHandData(ItemData.Slots[Slot].SlotType, Result))
		{
			ItemData.Slots[Slot].ActorRef->SetActorEnableCollision(false);
			ItemData.Slots[Slot].ActorRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Result.UnequipSocketAttach);
			ItemData.Slots[Slot].ActorRef->OnDestroyed.AddUniqueDynamic(this, &UBaseHandItemManager::ItemDestroyed);
			ItemData.Slots[Slot].ActorRef->SetActorRelativeTransform(Result.UnequipSocketOffset);
		}
	}
	OnHandItemManagerUpdate.Broadcast(this);
	
	return true;
}

void UBaseHandItemManager::Server_SetupItem_Implementation(UBaseItem* Target, int32 Slot)
{
	SetupItem(Target, Slot);
}

void UBaseHandItemManager::OnRep_Slots()
{
	OnHandItemManagerUpdate.Broadcast(this);
	OnSlotsUpdate.Broadcast(this);
	for (auto Data : ItemData.Slots)
	{
		if(!IsValid(Data.ActorRef)) continue;
		Data.ActorRef->SetActorEnableCollision(false);
	}
	if (!HasAuthority())
		OnRep_State();
}


#pragma endregion //SetupItem

#pragma region RemoveItem

bool UBaseHandItemManager::RemoveItemByItem(UBaseItem* Target)
{
	for (auto& LSlotData : ItemData.Slots)
	{
		if (IsValid(LSlotData.ActorRef) && LSlotData.ActorRef->GetItem() == Target)
		{
			if (LSlotData.ActorRef == GetCurrentActor() && IsValid(GetCurrentActor()))
			{
				if (LSlotData.ItemRef)
				{
					if (UBaseHandItemService* HandItemService = LSlotData.ItemRef->GetServiceTyped<UBaseHandItemService>(UBaseHandItemService::StaticClass()))
					{
						HandItemService->DoUnequipLogic(GetOwningPlayerPawn());
					}
				}
				IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
				if (LControllInterface == nullptr) return false;
				LControllInterface->Execute_I_UnequipActor(GetCurrentActor());
				if (LSlotData.ItemRef == HandItemState.TargetItem)
				{
					HandItemState.TargetItem = nullptr;
				}
				HandItemState.CurrentItem = nullptr;
			}
			auto ToDestroyActor = LSlotData.ActorRef;
			LSlotData.ActorRef = nullptr;
			LSlotData.ItemRef = nullptr;
			if (IsValid(ToDestroyActor))
			{
				ToDestroyActor->Destroy();
			}
			OnHandItemManagerUpdate.Broadcast(this);
			return true;
		}
	}
	return true;
}

#pragma endregion //RemoveItem

#pragma region FOV

void UBaseHandItemManager::InterpolateFOVTo()
{
	if (!GetOwningPlayerPawn()) return;

	ABaseFireWeapon* FireWeapon = Cast<ABaseFireWeapon>(GetCurrentActor());
	if (!IsValid(FireWeapon)) return;

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!IsValid(PlayerController) || (IsValid(PlayerController) && !IsValid(PlayerController->PlayerCameraManager))) return;

	float Time = GetWorld()->GetTimeSeconds() - PreviewTimeFOV;
	PreviewTimeFOV = GetWorld()->GetTimeSeconds();

	float TargetFOV = FireWeapon->GetFireWeaponData()->FOVAiming - PlayerController->PlayerCameraManager->DefaultFOV;
	TargetFOV *= (Time * FireWeapon->GetFireWeaponData()->SpeedInterpolateFOVFrom);
	PlayerController->PlayerCameraManager->SetFOV(PlayerController->PlayerCameraManager->GetFOVAngle() + TargetFOV);

	bool bIsStopFOV = false;
	bIsStopFOV |= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerController->PlayerCameraManager->GetFOVAngle(), FireWeapon->GetFireWeaponData()->FOVAiming, 0.5f);
	bIsStopFOV |= (PlayerController->PlayerCameraManager->GetFOVAngle() < FireWeapon->GetFireWeaponData()->FOVAiming);

	if (bIsStopFOV)
	{
		PlayerController->PlayerCameraManager->SetFOV(FireWeapon->GetFireWeaponData()->FOVAiming);
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_InterpolateFovTo))
			GetWorld()->GetTimerManager().ClearTimer(Timer_InterpolateFovTo);
	}
}

void UBaseHandItemManager::InterpolateFOVFrom()
{
	if (!GetOwningPlayerPawn()) return;

	ABaseFireWeapon* FireWeapon = Cast<ABaseFireWeapon>(GetCurrentActor());
	if (!IsValid(FireWeapon)) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayerPawn()->GetController());
	if (!IsValid(PlayerController) || (IsValid(PlayerController) && !IsValid(PlayerController->PlayerCameraManager))) return;

	float Time = GetWorld()->GetTimeSeconds() - PreviewTimeFOV;
	PreviewTimeFOV = GetWorld()->GetTimeSeconds();

	float TargetFOV = PlayerController->PlayerCameraManager->DefaultFOV - FireWeapon->GetFireWeaponData()->FOVAiming;

	TargetFOV *= (Time * FireWeapon->GetFireWeaponData()->SpeedInterpolateFOVFrom);
	PlayerController->PlayerCameraManager->SetFOV(PlayerController->PlayerCameraManager->GetFOVAngle() + TargetFOV);

	bool bIsStopFOV = false;
	bIsStopFOV |= UKismetMathLibrary::NearlyEqual_FloatFloat(PlayerController->PlayerCameraManager->GetFOVAngle(), PlayerController->PlayerCameraManager->DefaultFOV, 0.5f);
	bIsStopFOV |= (PlayerController->PlayerCameraManager->GetFOVAngle() > PlayerController->PlayerCameraManager->DefaultFOV);

	if (bIsStopFOV)
	{
		PlayerController->PlayerCameraManager->SetFOV(PlayerController->PlayerCameraManager->DefaultFOV);
		if (GetWorld()->GetTimerManager().IsTimerActive(Timer_InterpolateFovFrom))
			GetWorld()->GetTimerManager().ClearTimer(Timer_InterpolateFovFrom);
	}
}

#pragma endregion //FOV

#pragma region TOOLS

void UBaseHandItemManager::SwitchScope(bool bUse)
{
	if (!IsValid(GetCurrentActor())) return;

	IInventoryActorControll* LControllInterface = UHandItemLibrary::HasControllInterface(GetCurrentActor());
	if (LControllInterface == nullptr) return;

	LControllInterface->Execute_I_SwitchScope(GetCurrentActor(), bUse);
}

UBaseItem* UBaseHandItemManager::GetItemAtIndex(int32 Idx)
{
	if(!ItemData.Slots.IsValidIndex(Idx))	return nullptr;

	return (IsValid(ItemData.Slots[Idx].ItemRef) ? ItemData.Slots[Idx].ItemRef : nullptr);
}

ABaseItemActor* UBaseHandItemManager::GetItemActorAtIndex(int32 Idx)
{
	if (!ItemData.Slots.IsValidIndex(Idx))	return nullptr;

	return (IsValid(ItemData.Slots[Idx].ActorRef) ? ItemData.Slots[Idx].ActorRef : nullptr);
}

UBaseHandItemManager* UBaseHandItemManager::GetBaseHandItemManager(AActor* FromActor)
{
	if (!IsValid(FromActor)) return nullptr;
	return Cast<UBaseHandItemManager>(FromActor->GetComponentByClass(UBaseHandItemManager::StaticClass()));
}


ABaseItemActor* UBaseHandItemManager::GetCurrentActor()
{
	if (!IsValid(HandItemState.CurrentItem)) return nullptr;
	UBaseWorldItemService* WorldService = (IsValid(HandItemState.CurrentItem) ? HandItemState.CurrentItem->GetWorldPlacementService() : nullptr);
	if (!WorldService) return nullptr;

	return (WorldService->GetSpawnedRef());
}

FHandItemState UBaseHandItemManager::GetHandItemState()
{
	return HandItemState;
}

bool UBaseHandItemManager::InActiveAction()
{
    return IsPlayEquipMontagne;
}
int32 UBaseHandItemManager::GetSlotIdByTag(FGameplayTag InTag)
{
	for (int32 Idx = 0; Idx < ItemData.Slots.Num(); Idx++)
	{
		if (ItemData.Slots[Idx].SlotType == InTag) return Idx;
	}
    return INDEX_NONE;
}

int32 UBaseHandItemManager::GetSlotIdByItem(UBaseItem* ItemRef)
{
	for (int32 Idx = 0; Idx < ItemData.Slots.Num(); Idx++)
	{
		if (IsValid(ItemData.Slots[Idx].ActorRef) && ItemData.Slots[Idx].ActorRef->GetItem() == ItemRef) return Idx;
	}
	return INDEX_NONE;
}

int32 UBaseHandItemManager::GetSlotIdByItemActor(ABaseItemActor* ItemRef)
{
	for (int32 Idx = 0; Idx < ItemData.Slots.Num(); Idx++)
	{
		if (IsValid(ItemData.Slots[Idx].ActorRef) && ItemData.Slots[Idx].ActorRef == ItemRef) return Idx;
	}
	return INDEX_NONE;
}

void UBaseHandItemManager::ItemDestroyed(AActor* ItemRef)
{
	if (!IsValid(ItemRef)) return;
	if (ABaseItemActor* ItemActor = Cast<ABaseItemActor>(ItemRef))
	{
		RemoveItemByItem(ItemActor->GetItem());
	}
}
#pragma endregion TOOLS