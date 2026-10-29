// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInventoryManager.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseSpatialInventory.h"
#include "InventoryEcosystem/InventoryManagerSystem/Object/Service/BaseInventoryItemService.h"


#pragma region ENGINE

UBaseInventoryManager::UBaseInventoryManager(){}


void UBaseInventoryManager::BeginPlay()
{
	Super::BeginPlay();
}

void UBaseInventoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UBaseInventoryManager, Inventoryes, COND_OwnerOnly);
}

void UBaseInventoryManager::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar); return;
	/*if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }

	if (Ar.IsSaving())
	{
		SavedData = FInventoryManagerSave(this);
		Super::Serialize(Ar);
		Ar << SavedData;
		return;
	}

	Super::Serialize(Ar);
	Ar << SavedData;

	for (auto SaveData : SavedData.Slots)
	{
		FClothInventoryData NewInventory;
		NewInventory.Inventory = NewObject<UBaseSpatialInventory>(GetOwner());
		NewInventory.Inventory->RegisterComponentWithWorld(GetWorld());

		NewInventory.ItemOwner = SaveData.ItemOwner;
		NewInventory.InventoryName = SaveData.InventoryName;
		if (IsValid(NewInventory.Inventory))
		{
			NewInventory.Inventory->OnUpdateInventory.AddUniqueDynamic(this, &UInventoryManager::UpdateInventory);
			NewInventory.Inventory->OnUpdateInventoryItems.AddUniqueDynamic(this, &UInventoryManager::UpdateInventory);
		}

		FMemoryReader MemoryReader(SaveData.InventoryData, true);
		FSaveGameArchive ArLoad(MemoryReader);
		NewInventory.Inventory->Serialize(ArLoad);
		Inventoryes.Add(NewInventory);
	}*/
}

#pragma endregion ENGINE

#pragma region ADD_INVENTORY

bool UBaseInventoryManager::AddInventory(UBaseItem* Ref)
{
	if (FindInventory(Ref)) return true;
	if (!IsValid(Ref)) return false;

	UBaseInventoryItemService* InventoryService = Ref->GetServiceTyped<UBaseInventoryItemService>(UBaseInventoryItemService::StaticClass());
	if (!InventoryService) return false;

	if (!HasAuthority())
	{
		Server_AddInventory(Ref);
		return true;
	}

	//LOG_INVENTORYMANAGER(EInventoryLogType::EILog_Display, "Start add new inventory");

	FClothInventoryData NewInventory;
	NewInventory.Inventory = InventoryService->MakeInventory(GetOwner());
	NewInventory.ItemOwner = Ref;
	Inventoryes.Add(NewInventory);
	if (IsValid(NewInventory.Inventory))
	{
		// LOG_INVENTORYMANAGER(EInventoryLogType::EILog_Display, "Success add new inventory");
		NewInventory.Inventory->OnUpdateInventory.AddUniqueDynamic(this, &UBaseInventoryManager::UpdateInventory);
		NewInventory.Inventory->OnUpdateInventoryItems.AddUniqueDynamic(this, &UBaseInventoryManager::UpdateInventory);
	}

	OnRep_Inventory();
	return true;
}

void UBaseInventoryManager::Server_AddInventory_Implementation(UBaseItem* Ref)
{
	AddInventory(Ref);
}

#pragma endregion ADD_INVENTORY

#pragma region REMOVE_INVENTORY

bool UBaseInventoryManager::RemoveInventory(UBaseItem* Ref)
{
	if (!IsValid(Ref)) return true;

	if (!HasAuthority())
	{
		Server_RemoveInventory(Ref);
		return true;
	}

	for (int32 Idx = 0; Idx < Inventoryes.Num(); Idx++)
	{
		if (!IsValid(Inventoryes[Idx].ItemOwner)) continue;
		if (Inventoryes[Idx].ItemOwner != Ref) continue;

		if (IsValid(Inventoryes[Idx].Inventory))
		{
			if (Inventoryes[Idx].Inventory->OnUpdateInventoryItems.IsAlreadyBound(this, &UBaseInventoryManager::UpdateInventory))
			{
				Inventoryes[Idx].Inventory->OnUpdateInventory.RemoveDynamic(this, &UBaseInventoryManager::UpdateInventory);
			}

			if (Inventoryes[Idx].Inventory->OnUpdateInventoryItems.IsAlreadyBound(this, &UBaseInventoryManager::UpdateInventory))
			{
				Inventoryes[Idx].Inventory->OnUpdateInventoryItems.RemoveDynamic(this, &UBaseInventoryManager::UpdateInventory);
			}
		}
		Inventoryes.RemoveAt(Idx);
		OnRep_Inventory();
		return true;
	}
	return true;
}

void UBaseInventoryManager::Server_RemoveInventory_Implementation(UBaseItem* Ref)
{
	RemoveInventory(Ref);
}

#pragma endregion REMOVE_INVENTORY

#pragma region TOOLS

bool UBaseInventoryManager::CanAddItem(UBaseItem* Ref)
{
	if (!IsValid(Ref)) return false;
	
	for (auto Inventory : Inventoryes)
	{
		if (!IsValid(Inventory.Inventory)) continue;
		
		if (!Inventory.Inventory->CanAddItem(Ref)) continue;

		return true;			
	}
	
	return false;
}

UBaseLineInventory* UBaseInventoryManager::FindInventory(UBaseItem* Ref)
{
	if (!IsValid(Ref)) return nullptr;

	for (int32 Idx = 0; Idx < Inventoryes.Num(); Idx++)
	{
		if (!IsValid(Inventoryes[Idx].ItemOwner)) continue;
		if (Inventoryes[Idx].ItemOwner != Ref) continue;
		return Inventoryes[Idx].Inventory;
	}
	return nullptr;
}

void UBaseInventoryManager::OnRep_Inventory()
{
	OnUpdateInventoryes.Broadcast(this);
	OnInventoryManagerUpdate.Broadcast(this);
}

void UBaseInventoryManager::UpdateInventory(UBaseLineInventory* InventoryRef)
{
	OnInventoryManagerUpdate.Broadcast(this);
}


int32 UBaseInventoryManager::GetAllCountByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass)
{
	int32 Result = 0;
	for (auto InventoryData : Inventoryes)
	{
		Result += InventoryData.Inventory->GetAllCountByClass(ItemClass, IsCheckOnParrentClass);
	}
	return Result;
}
int32 UBaseInventoryManager::SubstructItem(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass)
{
	if (!HasAuthority()) return INDEX_NONE;
	
	int32 Result = 0;
	int32 Target = TargetAmount;
	int32 Current = 0;
	for (auto InventoryData : Inventoryes)
	{
		Current = InventoryData.Inventory->SubstructItem(ItemClass, TargetAmount, IsCheckOnParrentClass);
		Result += Current;
		Target -= Current;
		if (Target < 1) return Result;
	}
	return Result;
}
TArray<UBaseItem*> UBaseInventoryManager::GetAllItemByClass(TSubclassOf<UBaseItem> ItemClass, bool ReverseArray, bool IsCheckOnParrentClass)
{
	TArray<UBaseItem*> Result;
	for (auto InventoryData : Inventoryes)
	{
		Result.Append(InventoryData.Inventory->GetAllItemByClass(ItemClass, ReverseArray, IsCheckOnParrentClass));
	}
	return Result;
}

TArray<UBaseItem*> UBaseInventoryManager::GetAllItemByClassArray(TArray<TSubclassOf<UBaseItem>> ItemClass, bool OnlyOneByClass)
{
	TArray<UBaseItem*> Result;
	for (auto InventoryData : Inventoryes)
	{
		Result.Append(InventoryData.Inventory->GetAllItemByClassArray(ItemClass, OnlyOneByClass));
	}
	return Result;
}

TArray<UBaseItem*> UBaseInventoryManager::GetAllItemByType(FGameplayTag ItemType)
{
	TArray<UBaseItem*> Result;
	for (auto InventoryData : Inventoryes)
	{
		Result.Append(InventoryData.Inventory->GetAllItemByType(ItemType));
	}
	return Result;
}

bool UBaseInventoryManager::AddTryItem(UBaseItem* NewItem, bool IsNeedStackItem, bool WithDelegate)
{
	for (auto InventoryData : Inventoryes)
	{
		if (!InventoryData.Inventory->AddTryItem(NewItem, IsNeedStackItem, WithDelegate)) continue;
		return true;
	}
	return false;
}
TArray<UBaseItem*> UBaseInventoryManager::GetAllItem()
{
	TArray<UBaseItem*> Result;
	for (auto InventoryData : Inventoryes)
	{
		Result.Append(InventoryData.Inventory->GetAllItem());
	}
	return Result;
}

const TArray<FClothInventoryData>& UBaseInventoryManager::GetInventoryData()
{
	return Inventoryes;
}

UBaseInventoryManager* UBaseInventoryManager::GetBaseInventoryManager(AActor* From)
{
	if (!IsValid(From)) return nullptr;
	return Cast<UBaseInventoryManager>(From->GetComponentByClass<UBaseInventoryManager>());
}
#pragma endregion TOOLS
