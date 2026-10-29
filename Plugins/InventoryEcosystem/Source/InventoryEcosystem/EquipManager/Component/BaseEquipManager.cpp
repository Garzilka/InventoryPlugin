// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEquipManager.h"
// #include "InventorySystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/EquipManager/Objects/Services//BaseClothService.h"
#include "InventoryEcosystem/EquipManager/Library/EquipManagerTool.h"


#pragma region Engine

#if WITH_EDITOR
void UBaseEquipManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

UBaseEquipManager::UBaseEquipManager()
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

void UBaseEquipManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && !SavedData.IsValidData())
	{
		for (auto ItemData : DefaultItem)
		{
			SetClothItem(UInventoryLibrary::MakeItem(GetOwner(), ItemData.Value));
		}
		OnRep_EquipArray();
	}
}

void UBaseEquipManager::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }
	if (!HasAuthority()) { Super::Serialize(Ar); return; }
	
	// LOG_EQUIPMANAGER(EInventoryLogType::EILog_Display, "Serialize EQUIP_MANAGER: Run serialize");
	
	if (Ar.IsSaving())
	{
		// LOG_EQUIPMANAGER(EInventoryLogType::EILog_Display, "Serialize EQUIP_MANAGER: Save data");
		SavedData = FEquipManagerSave(this);
		Super::Serialize(Ar);
		Ar << SavedData;
		// for (auto Item : GetAllItem())
		// {
		// 	if (!Item) continue;
		// 	Item->Serialize(Ar);
		// }
		return;
	}
	
	Super::Serialize(Ar);
	Ar << SavedData;
	
	if (!SavedData.IsValidData())
	{
		// LOG_EQUIPMANAGER(EInventoryLogType::EILog_Display, "Serialize EQUIP_MANAGER: Hansn't save data");
		for (auto ItemData : DefaultItem)
		{
			SetClothItem(UInventoryLibrary::MakeItem(GetOwner(), ItemData.Value));
		}
		OnRep_EquipArray();
		return;
	}
	LoadDefault();
	// LOG_EQUIPMANAGER(EInventoryLogType::EILog_Display, "Serialize EQUIP_MANAGER: Start load data");
	for (auto ItemData : SavedData.SaveData)
	{
		UBaseItem* Item = UInventoryLibrary::MakeItem(GetOwner(), FInventoryItemData(ItemData.SavedClass));
		if (!IsValid(Item)) continue;
	
		FMemoryReader MemoryReader(ItemData.ItemData, true);
		FSaveGameArchive ArLoad(MemoryReader);
		Item->BeginPlay();
		Item->Serialize(ArLoad);
		SetClothItem(Item);
	}
	// LOG_EQUIPMANAGER(EInventoryLogType::EILog_Display, "Serialize EQUIP_MANAGER: load success");
	OnRep_EquipArray();
}

#pragma endregion

#pragma region Replicated

void UBaseEquipManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UBaseEquipManager, EquipArray, COND_OwnerOnly);
}

bool UBaseEquipManager::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto LItem : GetAllItem())
	{
		if (!LItem) continue;
		WroteSomething |= Channel->ReplicateSubobject(LItem, *Bunch, *RepFlags);
		for (auto AttachableItem : LItem->GetAttachedSubObject())
		{
			WroteSomething |= Channel->ReplicateSubobject(AttachableItem, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}


void UBaseEquipManager::OnRep_EquipArray()
{
	FGameplayTagContainer ClosedSlots;
	AvailableSlots.Reset();

	for (auto Item : GetAllItem())
	{
		if (!Item) continue;
		UBaseClothService* ClothService = Item->GetServiceTyped<UBaseClothService>();
		if (!ClothService) continue;

		// AvailableSlots.AppendTags(ClothService->GetOpenEquipSlots());
		// ClosedSlots.AppendTags(ClothService->GetCloseEquipSlots());

	}
	AvailableSlots.RemoveTags(ClosedSlots);
	OnEquipSlotsUpdate.Broadcast(this, AvailableSlots);
	RecalculateBody();
	OnUpdateEquipment.Broadcast(this);
}

void UBaseEquipManager::OnRep_FullArmorData()
{
	OnArmorDataUpdate.Broadcast(this);
}

void UBaseEquipManager::OnRep_BodyArmorData()
{
	OnBodyArmorDataUpdate.Broadcast(this);
}
#pragma endregion

#pragma region SetClothItem

bool UBaseEquipManager::SetClothItem(UBaseItem* TargetItem)
{
	if (!IsValid(TargetItem)) return false;
	if (!TargetItem->IsCanRemoveFromParrent()) return false;


	if (TargetItem->GetServiceTyped<UBaseClothService>() == nullptr) return false;

	if (!HasAuthority())
	{
		Server_SetClothItem(TargetItem);
		return true;
	}

	if (!TargetItem->SwitchParrent(this)) return false;
	TargetItem->ChangeOwner(GetOwner());
	UBaseClothService* LService = TargetItem->GetServiceTyped<UBaseClothService>();

	for (FGameplayTag Tag : LService->GetClothType())
	{
		UBaseItem* CurrentItem = GetClothItemByTag(Tag);
		if (!CurrentItem) continue;
		RemoveItemByItem(CurrentItem);
	}
	EquipArray.AddUnique(TargetItem);
	TargetItem->OnItemEquiped.Broadcast(TargetItem);
	
	if (LService)
	{
		LService->OnClothUpdate.AddUniqueDynamic(this, &UBaseEquipManager::ClothUpdate);
	}
	OnRep_EquipArray();

	return true;
}

#pragma endregion

#pragma region RemoveItemByItem

bool UBaseEquipManager::RemoveItemByItem(UBaseItem* TargetItem)
{
	if (!IsValid(TargetItem)) return false;

	int32 Idx = EquipArray.Find(TargetItem);
	if (Idx == INDEX_NONE) return false;

	if (HasAuthority())
	{
		if (UBaseClothService* Service = TargetItem->GetServiceTyped<UBaseClothService>(UBaseClothService::StaticClass()))
		{
			Service->OnClothUpdate.RemoveDynamic(this, &UBaseEquipManager::ClothUpdate);
		}
		EquipArray[Idx] = nullptr;
		TargetItem->OnItemUnequiped.Broadcast(TargetItem);
		OnRep_EquipArray();
	}
	else
	{
		Server_RemoveItemByItem(TargetItem);
	}
	return true;
}

#pragma endregion

#pragma region CalculateBody


void UBaseEquipManager::ClothUpdate(UBaseClothService* ClothUpdate)
{
	RecalculateBody();
}

void UBaseEquipManager::RecalculateBody()
{
	if (!GetWorld()) return;

	//RetriggerableDelay
	if (GetWorld()->GetTimerManager().IsTimerActive(RetriggerableDelay_RecalculateBody))
		GetWorld()->GetTimerManager().ClearTimer(RetriggerableDelay_RecalculateBody);
	GetWorld()->GetTimerManager().SetTimer(RetriggerableDelay_RecalculateBody, this, &UBaseEquipManager::StartBuildBody, 0.1f);
}

void UBaseEquipManager::StartBuildBody()
{
	ClearMeshComponentData();
	FGameplayTagContainer HideMask;

	for (auto* Cloth : GetAllItem())
	{
		if (!IsValid(Cloth)) continue;
		if (UBaseClothService* Service = Cloth->GetServiceTyped<UBaseClothService>())
		{
			HideMask.AppendTags(Service->GetHideBodyMask());
		}
	}

	for (auto Tag : UEquipManagerTool::GetBodyTags())
	{
		if (HideMask.HasTag(Tag)) continue;
		FMeshComponentData ComponentData;
		AddMesh(GetMeshByBodyType(Tag));
	}

	for (auto* ClothItem : GetAllItem())
	{
		if (!ClothItem) continue;
		UBaseClothService* ClothService = ClothItem->GetServiceTyped<UBaseClothService>();
		if (!ClothService) continue;

		auto LClothData = ClothService->GetMeshData(Gender);

		FMeshComponentData ComponentData;
		ComponentData.MeshRef = LClothData.MeshRef;
		ComponentData.MaterialMap = LClothData.MaterialMap;
		ComponentData.BodyTag = ClothService->GetBodyTag();
		AddMesh(ComponentData);

		for (auto AttachClothService : ClothService->GetAttaches())
		{
			auto LAttachClothData = AttachClothService->GetMeshData(Gender);
			
			FMeshComponentData LAttachComponentData;
			LAttachComponentData.MeshRef = LAttachClothData.MeshRef;
			LAttachComponentData.MaterialMap = LAttachClothData.MaterialMap;
			LAttachComponentData.BodyTag = AttachClothService->GetBodyTag();
			AddMesh(LAttachComponentData);
		}

	}
	BuildMesh();
}

#pragma endregion

#pragma region TOOLS

UBaseItem* UBaseEquipManager::GetClothItemByTag(FGameplayTag ClothTag)
{
	for (auto ClothItem : EquipArray)
	{
		if (!ClothItem) continue;
		UBaseClothService* Service = ClothItem->GetServiceTyped<UBaseClothService>();
		if (IsValid(Service) && Service->HasClothType(ClothTag)) return ClothItem;
	}

	return nullptr;
}

UBaseEquipManager* UBaseEquipManager::GetBaseEquipManager(AActor* FromActor)
{
	if (!IsValid(FromActor)) return nullptr;
	return Cast<UBaseEquipManager>(FromActor->GetComponentByClass(UBaseEquipManager::StaticClass()));
}

#pragma endregion