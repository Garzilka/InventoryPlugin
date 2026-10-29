// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClothService.h"

#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BasePickupService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"

#pragma region REPLICATED

void UBaseClothService::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseClothService, Attaches);
}

void UBaseClothService::OnRep_Attaches()
{
	OnClothUpdate.Broadcast(this);
}
#pragma endregion REPLICATED

#pragma region SAVE_LOAD


void UBaseClothService::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }
	//
	// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Start serialize");
	// if (Ar.IsSaving())
	// {
	// 	SaveAttaches.Reset();
	// 	for (auto Module : Attaches)
	// 	{
	// 		if (!(IsValid(Module) && IsValid(Module->GetItemOwner()))) continue;
	// 		FClothServiceSave ClothSave;
	// 		ClothSave.SaveItem = Module->GetClass();
	// 		ClothSave.SaveTag = Module->GetInstallTag();
	// 		SaveAttaches.Add(ClothSave);
	// 	}
	// 	LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Start safe");
	// 	Super::Serialize(Ar);
	//
	// 	LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Start module");
	// 	for (auto Module : Attaches)
	// 	{
	// 		if (!(IsValid(Module) && IsValid(Module->GetItemOwner()))) continue;
	// 		Module->GetItemOwner()->Serialize(Ar);
	// 	}
	//
	// 	return;
	// }
	//
	// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Start load");
	// Super::Serialize(Ar);
	//
	// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Loaded");
	// if (Ar.IsLoading())
	// {
	// 	LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "Make subattach");
	// 	for (auto SaveData : SaveAttaches)
	// 	{
	// 		UBaseItem* Item = UInventoryLibrary::MakeItem(GetOwner(), FInventoryItemData(SaveData.SaveItem));
	// 		if (!Item) continue;
	// 		AttachItem(Item, SaveData.SaveTag);
	// 		Item->Serialize(Ar);
	// 	}
	// }
	// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "SERIALIZE ITEM_SERVICE: Finish");
}

TArray<UObject*> UBaseClothService::GetAllChieldObject()
{
	TArray<UObject*> Result;
	for (auto ClothAttach : Attaches)
	{
		if (!(ClothAttach && ClothAttach->GetItemOwner())) continue;
		Result.AddUnique(ClothAttach->GetItemOwner());
		Result.Append(ClothAttach->GetItemOwner()->GetAttachedSubObject());
	}
	return Result;
}

TArray<UBaseItem*> UBaseClothService::GetAttachedItem()
{
	TArray<UBaseItem*> Result;
	for (auto Service : Attaches)
	{
		if (!IsValid(Service)) continue;
		Result.Add(Service->GetItemOwner());
		Result.Append(Service->GetItemOwner()->GetAttachedItem());
	}
	return Result;
}

#pragma endregion SAVE_LOAD

#pragma region ATTACH

bool UBaseClothService::AttachItem(UBaseItem* Item, FGameplayTag Tag)
{
	if (IsValid(Item) )
	{
		// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "INVALID ITEM");
		return false;
	}
	if (BaseSupportModification.HasTag(Tag))
	{
		// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "Not supported tag");
		return false;
	}

	UBaseAttachClothService* LClothService = Item->GetServiceTyped<UBaseAttachClothService>(UBaseAttachClothService::StaticClass());
	if (!IsValid(LClothService))
	{
		// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "Hasn't cloth service");
		return false;
	}


	if (!HasAuthority())
	{
		Server_AttachItem(Item, Tag);
		return true;
	}


	if (!Item->IsCanRemoveFromParrent()) return false;

	UBaseAttachClothService* LastAttach = GetAttachByAttachSlot(Tag);
	if (IsValid(LastAttach) && IsValid(LastAttach->GetItemOwner()))
	{
		if (LastAttach->GetItemOwner()->GetPickupService())
		{
			LastAttach->GetItemOwner()->GetPickupService()->PickupItem(GetOwningPlayerPawn());
		}
	}

	Item->SwitchParrent(this);
	Attaches.Add(LClothService);

	OnRep_Attaches();
	// LOG_ITEMSERVICE(EInventoryLogType::EILog_Display, "Attached");
	return true;
}

void UBaseClothService::Server_AttachItem_Implementation(UBaseItem* Item, FGameplayTag Tag)
{
	AttachItem(Item, Tag);
}

#pragma endregion ATTACH

#pragma region DETACH

bool UBaseClothService::DetachItem(UBaseItem* Item)
{
	if (!Item) return false;
	if (!HasAuthority())
	{
		Server_DetachItem(Item);
		return true;
	}

	if (UBaseAttachClothService* LClothService = Item->GetServiceTyped<UBaseAttachClothService>(UBaseAttachClothService::StaticClass()))
	{
		return DetachClothService(LClothService);
	}
	return false;
}

bool UBaseClothService::DetachClothService(UBaseAttachClothService* AttachCloth)
{
	if (!AttachCloth) return false;
	for (int32 Idx = 0; Idx < Attaches.Num(); Idx++)
	{
		if (Attaches[Idx] != AttachCloth) continue;
		Attaches.RemoveAt(Idx);
		OnRep_Attaches();
		return true;
	}
	return true;
}

void UBaseClothService::Server_DetachItem_Implementation(UBaseItem* Item)
{
	DetachItem(Item);
}

#pragma endregion DETACH

#pragma region TOOLS

UBaseAttachClothService* UBaseClothService::GetAttachByAttachSlot(FGameplayTag TagAttach)
{
	if (!TagAttach.IsValid()) return nullptr;

	for (auto Attach : Attaches)
	{
		if (!IsValid(Attach)) continue;
		if (Attach->GetInstallTag() != TagAttach) continue;
		return Attach;
	}

	return nullptr;
}


void UBaseClothService::BeginPlay(UBaseItem* Item)
{
	Super::BeginPlay(Item);

	if (!GetItemOwner()->GetWorldPlacementService()) return;
	GetItemOwner()->GetWorldPlacementService()->OnMeshLoadAndSet.AddUniqueDynamic(this, &UBaseClothService::Event_PreSpawnItem);
}

FGameplayTagContainer UBaseClothService::GetOpenEquipSlots()
{
	FGameplayTagContainer Result = OpenEquipSlots;
	for (auto AttachService : Attaches)
	{
		if (!AttachService) continue;
		Result.AppendTags(AttachService->GetOpenEquipSlots());
	}
	return Result;
}

FGameplayTagContainer UBaseClothService::GetCloseEquipSlots()
{
	FGameplayTagContainer Result = CloseEquipSlots;
	for (auto AttachService : Attaches)
	{
		if (!AttachService) continue;
		Result.AppendTags(AttachService->GetCloseEquipSlots());
	}
	return Result;
}

bool UBaseClothService::HasClothType(FGameplayTag InType)
{
	return ClothType.HasTag(InType);
}

FClothData UBaseClothService::GetMeshData(FName Gender)
{
	auto* Ref = ClothGenderData.Find(Gender);
	if (Ref == nullptr)	return FClothData();
	return *Ref;
}

void UBaseClothService::Event_PreSpawnItem(UBaseWorldItemService* WorldService, ABaseItemActor* SpawnedReference)
{
	if (!(GetItemOwner() && IsValid(SpawnedReference))) return;

	// UStaticMeshComponent* ItemMesh = SpawnedReference->GetMesh();
	// if (ItemMesh->GetStaticMesh() == nullptr) return;
	//
	// FGenderMeshSkinData LGenderMeshSkinData = GetMeshData("Male");
	// auto LData = LGenderMeshSkinData.SkinMaterials.Find(GetItemOwner()->GetCurrentSkin());
	// if (LData == nullptr) return;
	//
	// for (auto MatData : LData->MeshSlotMaterialAndMaterial)
	// {
	// 	int32 IdSlot = ItemMesh->GetStaticMesh()->GetMaterialIndex(FName(MatData.Key));
	//
	// 	if (IdSlot == INDEX_NONE) continue;
	//
	// 	MatData.Value.LoadSynchronous();
	// 	ItemMesh->SetMaterialByName(FName(MatData.Key), MatData.Value.Get());
	// }
}


#pragma endregion TOOLS

