// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLineInventory.h"

#include "TimerManager.h"
#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "Engine/World.h"


#pragma region ENGINE

UBaseLineInventory::UBaseLineInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FInventorySave UBaseLineInventory::GetSaveData()
{
	SaveData();
	return SavedData;
}

void UBaseLineInventory::BeginPlay()
{
	Super::BeginPlay();

	if (!SavedData.IsValidData())
	{
		TArray<UBaseItem*> Ref;
		SetSize(DefaultInventorySize, Ref);
		return;
	}
}

void UBaseLineInventory::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }

	if (Ar.IsSaving())
	{
		SaveData();
		Super::Serialize(Ar);
		Ar << SavedData;
		for (auto Item : GetAllItem())
		{
			if (!IsValid(Item)) continue;
			Item->Serialize(Ar);
		}
		return;
	}

	Super::Serialize(Ar);
	Ar << SavedData;


	if (!SavedData.IsValidData())
	{
		TArray<UBaseItem*> Ref;
		SetSize(DefaultInventorySize, Ref);
		// LoadDefault();
		return;
	}

	if (!HasAuthority()) return;
	
	TArray<UBaseItem*> Items;
	SetSize(FIntPoint(0), Items);
	for (auto Item : Items) //InvalidateData
	{
		Item->MarkAsGarbage();
	}
	LoadSaveData(SavedData);
}


void UBaseLineInventory::LoadSaveData(FInventorySave Save)
{
    ClearInventory();
	
    TArray<UBaseItem*> Items;
    SetSize(SavedData.Size, Items);

    for (int32 Idx = 0; Idx < SavedData.SavedData.Num(); Idx++)
    {
        for (auto Item : UInventoryLibrary::MakeItems(GetOwner(), FInventoryItemData(SavedData.SavedData[Idx].SavedClass)))
        {
            if (!IsValid(Item)) continue;
            AddTryItem(Item, true, false);
            FMemoryReader MemoryReader(SavedData.SavedData[Idx].ItemData, true);
            FSaveGameArchive ArLoad(MemoryReader);
            Item->BeginPlay();
            Item->Serialize(ArLoad);
        }
    }
}

void UBaseLineInventory::SaveData()
{
	SavedData = FInventorySave(GetAllItemGrid(), GetOwner());
	SavedData.Size = InventoryData.GetSize();
}
#pragma endregion ENGINE

#pragma region Replicated

void UBaseLineInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseLineInventory, InventoryData);
	DOREPLIFETIME(UBaseLineInventory, InventoryWeight);
}

bool UBaseLineInventory::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto LItem : GetAllItem())
	{
		if (!IsValid(LItem)) continue;

		WroteSomething |= Channel->ReplicateSubobject(LItem, *Bunch, *RepFlags);

		for (auto UObject : LItem->GetAttachedSubObject())
		{
			if (!IsValid(UObject)) continue;

			WroteSomething |= Channel->ReplicateSubobject(UObject, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}



void UBaseLineInventory::OnRep_Inventory()
{
	ItemCache.InvalidateCache();

	//RetriggerableDalay
	if (GetWorld()->GetTimerManager().IsTimerActive(OnRepRemaining))
		GetWorld()->GetTimerManager().ClearTimer(OnRepRemaining);
	GetWorld()->GetTimerManager().SetTimer(OnRepRemaining, this, &UBaseLineInventory::InventoryUpdate, 0.01f, false);
}

void UBaseLineInventory::InventoryUpdate()
{
	if (!HasAuthority())
	{
		for (auto Item : GetAllItem())
		{
			BindItem(Item);
		}
	}
	ItemCache.InvalidateCache();
	OnUpdateInventory.Broadcast(this);
}

void UBaseLineInventory::OnRep_InventoryWeight()
{
}
#pragma endregion

#pragma region AddTryItem

bool UBaseLineInventory::AddTryItem(UBaseItem* InItem, bool IsNeedStackItem, bool WithDelegate)
{
	if (InItem == nullptr)	return false;
	if (!InItem->IsCanRemoveFromParrent()) return false;
	if (Contains(InItem)) return true;

	if (!HasAuthority())
	{
		if (!InItem->IsCanRemoveFromParrent()) return false;
		Server_AddTryItem(InItem, IsNeedStackItem, WithDelegate);
		return true;
	}

	auto CachedInventory = InventoryData.Inventory;

	for (int32 i = 0; i < InventoryData.Inventory.Num(); i++)
	{
		if (!IsPositionAvailable(InItem, i, IsNeedStackItem)) continue;
		if (!AddItemAt(InItem, i, IsNeedStackItem)) continue;

		if (!InItem->SwitchParrent(this))
		{
			InventoryData.Inventory = CachedInventory;
			return false;
		}

		OnRep_Inventory();
		if (WithDelegate)	OnNewItem.Broadcast(InItem, ConvertIndexToPosition(i), WithDelegate);
		return true;
	}
	return false;
}

void UBaseLineInventory::Server_AddTryItem_Implementation(UBaseItem* InItem, bool IsNeedStackItem, bool WithDelegate)
{
	AddTryItem(InItem, IsNeedStackItem, WithDelegate);
}
#pragma endregion

#pragma region AddItemAt

bool UBaseLineInventory::AddItemAt(UBaseItem* InItem, int32 TopLeftIndex, bool IsNeedStackItem)
{
	if (!(InItem && InventoryData.Inventory.IsValidIndex(TopLeftIndex))) return false;


	if (!HasAuthority())
	{
		if (!InItem->IsCanRemoveFromParrent()) return false;
		Server_AddItemAt(InItem, TopLeftIndex, IsNeedStackItem);
		return true;
	}

	if (GetItemAtIndex(TopLeftIndex) && GetItemAtIndex(TopLeftIndex)->StackItems(InItem))
	{
		if (!IsValid(InItem))
		{
			return true;
		}
	}

	if (FIntPoint* Position = GetAllItemGrid().Find(InItem))
	{
		int32 From = (Position->X * Position->Y);
		return MoveItem(From, TopLeftIndex);
	}

	ChangeObjectOuter(InItem, GetOwner());

	if (!InItem->SwitchParrent(this)) return false;

	InventoryData.Inventory[TopLeftIndex] = InItem;
	BindItem(InItem);

	return true;
}
void UBaseLineInventory::Server_AddItemAt_Implementation(UBaseItem* InItem, int32 TopLeftIndex, bool IsNeedStackItem)
{
	if (!IsValid(InItem)) return;
	AddItemAt(InItem, TopLeftIndex, IsNeedStackItem);
}

#pragma endregion

#pragma region RemoveItem

bool UBaseLineInventory::RemoveItemByItem(UBaseItem* InItem, bool WithDelegate)
{
	if (!IsValid(InItem)) return false;

	int32 LIdx = InventoryData.Inventory.Find(InItem);
	if (LIdx == INDEX_NONE) return false;

	if (!HasAuthority())
	{
		Server_RemoveItemAt(LIdx, WithDelegate);
		return true;
	}
	while (LIdx != INDEX_NONE)
	{
		RemoveItemAt(LIdx, WithDelegate);
		LIdx = InventoryData.Inventory.Find(InItem);
	}
	return true;
}

bool UBaseLineInventory::RemoveItemAt(int32 TopLeftIndex, bool WithDelegate)
{
	if (!GetOwner()) return false;
	if (GetItemAtIndex(TopLeftIndex) == nullptr) return false;

	if (!HasAuthority())
	{
		Server_RemoveItemAt(TopLeftIndex, WithDelegate);
		return true;
	}

	UBaseItem* LItem = InventoryData.Inventory[TopLeftIndex];
	InventoryData.Inventory[TopLeftIndex] = nullptr;

	if (WithDelegate)
	{
		OnLeaveItem.Broadcast(this, LItem, true);
	}
	UnbindItem(LItem);
	ItemCache.InvalidateCache();

	return true;
}



void UBaseLineInventory::Server_RemoveItemAt_Implementation(int32 TopLeftIndex, bool WithDelegate)
{
	RemoveItemAt(TopLeftIndex, WithDelegate);
}
#pragma endregion

#pragma region SubstructItem

int32 UBaseLineInventory::SubstructItem(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass)
{
	if (HasAuthority())
	{
		TArray<UBaseItem*> Items = GetAllItemByClass(ItemClass);
		int32 TargetSubstruct = TargetAmount;
		for (auto Item : Items)
		{
			if (Item->IsStackable())
			{
				if (TargetSubstruct >= Item->GetAmount())
				{
					TargetSubstruct -= Item->GetAmount();
					RemoveItemByItem(Item);
				}
				else
				{
					int32 AmountItem = (Item->GetAmount() - TargetSubstruct);
					TargetSubstruct -= Item->GetAmount();
					Item->SetAmount(AmountItem);
					if (TargetSubstruct <= 0)
					{
						TargetSubstruct = 0;
						break;
					}
				}

			}
			else
			{
				TargetSubstruct--;
				RemoveItemByItem(Item);
				if (TargetSubstruct < 0)
				{
					TargetSubstruct = 0;
					break;
				}
			}
		}

		return (TargetAmount - TargetSubstruct);
	}
	else
	{
		Server_SubstructItem(ItemClass, TargetAmount, IsCheckOnParrentClass);
		int32 Result = GetAllCountByClass(ItemClass, IsCheckOnParrentClass);
		return (Result > TargetAmount ? TargetAmount : Result);
	}
}

void UBaseLineInventory::Server_SubstructItem_Implementation(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass)
{
	SubstructItem(ItemClass, TargetAmount, IsCheckOnParrentClass);
}
#pragma endregion //SubstructItem

#pragma region Swap

bool UBaseLineInventory::CanSwap(int32 TopLeftIndexFrom, int32 TopLeftIndexTo)
{
	if (GetItemAtIndex(TopLeftIndexFrom) == nullptr || GetItemAtIndex(TopLeftIndexTo) == nullptr) return false;
	return true;
}

bool UBaseLineInventory::CanAddItem(UBaseItem* InItem, int32 Amount, bool WithStack)
{
	return true;
}

bool UBaseLineInventory::SwapItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo)
{
	if (!CanSwap(TopLeftIndexFrom, TopLeftIndexTo)) return false;

	if (!HasAuthority())
	{
		Server_SwapItem(TopLeftIndexFrom, TopLeftIndexTo);
		return true;
	}

	UBaseItem* Temp = GetItemAtIndex(TopLeftIndexFrom);
	InventoryData.Inventory[TopLeftIndexFrom] = GetItemAtIndex(TopLeftIndexTo);
	InventoryData.Inventory[TopLeftIndexTo] = Temp;
	return true;
}
void UBaseLineInventory::Server_SwapItem_Implementation(int32 TopLeftIndexFrom, int32 TopLeftIndexTo)
{
	SwapItem(TopLeftIndexFrom, TopLeftIndexTo);
}
#pragma endregion

#pragma region Move

bool UBaseLineInventory::MoveItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo)
{
	if (TopLeftIndexFrom == TopLeftIndexTo) return true;

	UBaseItem* ItemRef = GetItemAtIndex(TopLeftIndexFrom);
	if (!IsValid(ItemRef)) return false;
	UBaseItem* SecondItem = GetItemAtIndex(TopLeftIndexFrom);

	if (IsValid(SecondItem) && ItemRef != SecondItem)
	{
		return SwapItem(TopLeftIndexFrom, TopLeftIndexTo);
	}

	if (!IsPositionAvailable(ItemRef, TopLeftIndexTo, false)) return false;

	if (HasAuthority())
	{
		RemoveItemByItem(ItemRef, false);
		AddItemAt(ItemRef, TopLeftIndexTo, false);
		OnRep_Inventory();
	}
	else
	{
		Server_MoveItem(TopLeftIndexFrom, TopLeftIndexTo);
	}
	return true;
}

void UBaseLineInventory::Server_MoveItem_Implementation(int32 TopLeftIndexFrom, int32 TopLeftIndexTo)
{
	MoveItem(TopLeftIndexFrom, TopLeftIndexTo);
}


#pragma endregion

#pragma region TOOLS

void UBaseLineInventory::ClearInventory()
{
	if (!HasAuthority())
	{
		return;
	}
	
	for (auto& Item : GetAllItem())
	{
		if (!IsValid(Item)) continue;
		Item->MarkAsGarbage();
	}

	for (auto& Cell : InventoryData.Inventory)
	{
		Cell = nullptr;
	}
}

UBaseLineInventory* UBaseLineInventory::GetLineInventory(AActor* FromActor)
{
	if (!IsValid(FromActor)) return nullptr;
	return Cast<UBaseLineInventory>(FromActor->GetComponentByClass(UBaseLineInventory::StaticClass()));
}

bool UBaseLineInventory::IsPositionAvailable(UBaseItem* ItemRef, int32 Index, bool IsNeedStackItems)
{
	if (IsPositionValid(ConvertIndexToPosition(Index)))
	{
		if (UBaseItem* LItem = GetItemAtIndex(Index))
		{
			if (!IsNeedStackItems)
			{
				return false;
			}

			if (LItem->IsStackable())
			{
				return LItem->GetClass() == ItemRef->GetClass();
			}
			else if (LItem == ItemRef)
			{
				return true; //Collision with self
			}

			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

TArray<UBaseItem*> UBaseLineInventory::GetAllItem()
{
	if (ItemCache.IsValidCache) return ItemCache.Cache;

	ItemCache.Cache.Reset();
	TArray<UBaseItem*> CheckCacheArray;

	for (int32 Idx = 0; Idx < InventoryData.Inventory.Num(); Idx++)
	{
		if (!(InventoryData.Inventory.IsValidIndex(Idx) && InventoryData.Inventory[Idx] != nullptr)) continue;
		if (AsGrid && CheckCacheArray.Contains(InventoryData.Inventory[Idx])) continue;
		
		CheckCacheArray.Add(InventoryData.Inventory[Idx]);
		ItemCache.Cache.Add(InventoryData.Inventory[Idx]);
	}

	ItemCache.IsValidCache = true;
	return CheckCacheArray;
}

TArray<UBaseItem*> UBaseLineInventory::GetAllItemByClass(TSubclassOf<UBaseItem> ItemClass, bool ReverseArray, bool IsCheckOnParrentClass)
{
	if (!UKismetSystemLibrary::IsValid(ItemClass)) return {};

	TArray<UBaseItem*> LResult;
	for (auto Item : GetAllItem())
	{
		if (IsCheckOnParrentClass ? Item->IsA(ItemClass) : (Item->GetClass() == ItemClass))
		{
			LResult.Add(Item);
		}
	}

	if (ReverseArray) Algo::Reverse(LResult);

	return LResult;
}

TArray<UBaseItem*> UBaseLineInventory::GetAllItemByClassArray(TArray<TSubclassOf<UBaseItem>> ItemClassArray, bool OnlyOneByClass)
{
	TArray<UBaseItem*> Result;
	TArray<TSubclassOf<UBaseItem>> CheckArray;

	for (auto Item : GetAllItem())
	{
		if (!(IsValid(Item) && ItemClassArray.Contains(Item->GetClass()))) continue;
		if (OnlyOneByClass && CheckArray.Contains(Item->GetClass())) continue;

		Result.Add(Item);

		if (OnlyOneByClass)
		{
			CheckArray.Add(Item->GetClass());
		}
	}

	return Result;
}

TArray<UBaseItem*> UBaseLineInventory::GetAllItemByType(FGameplayTag ItemType)
{
	if (!ItemType.IsValid()) return {};

	TArray<UBaseItem*> LResult;
	for (auto Item : GetAllItem())
	{
		if (Item->GetItemType() != ItemType) continue;
		LResult.Add(Item);
	}

	return LResult;
}

TMap<UBaseItem*, FIntPoint> UBaseLineInventory::GetAllItemGrid()
{
	if (ItemCache.IsValidGridCache) return ItemCache.GridCache;

	ItemCache.GridCache.Reset();

	TArray<UBaseItem*> CheckCacheArray;
	int32 Idx = -1;
	for (auto Item : InventoryData.Inventory)
	{
		Idx++;
		if (!(InventoryData.Inventory.IsValidIndex(Idx) && InventoryData.Inventory[Idx] != nullptr)) continue;
		if (!(AsGrid && !CheckCacheArray.Contains(InventoryData.Inventory[Idx]))) continue;

		ItemCache.GridCache.Add(Item, ConvertIndexToPosition(Idx));
		CheckCacheArray.Add(Item);
	}

	ItemCache.IsValidGridCache = true;
	return ItemCache.GridCache;
}

int32 UBaseLineInventory::GetAllCountByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass)
{
	int32 Result = 0;
	for (auto Item : GetAllItem())
	{
		if (!(IsCheckOnParrentClass ? Item->IsA(ItemClass) : (Item->GetClass() == ItemClass))) continue;
		Result += Item->GetAmount();
	}
	return Result;
}


UBaseItem* UBaseLineInventory::GetFirstItemByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass)
{
	for (auto LItem : GetAllItem())
	{
		if (LItem->GetClass() != ItemClass) continue;
		return LItem;
	}
	return nullptr;
}

bool UBaseLineInventory::SetSize(FIntPoint NewSize, TArray<UBaseItem*>& Result)
{
	if (!HasAuthority()) return false;

	Result = GetAllItem();
	InventoryData.Inventory.Reset();
	InventoryData.SetSize(NewSize);
	InventoryData.Inventory.SetNum(InventoryData.GetNumBySize());

	for (int32 Idx = 0; Idx < Result.Num(); Idx++)
	{
		if (AddTryItem(Result[Idx], true, false))
		{
			Result.RemoveAt(Idx);
			Idx--;
		}
	}

	return true;
}

void UBaseLineInventory::BindItem(UBaseItem* ItemRef)
{
	if (!ItemRef) return;
	ItemRef->OnItemUpdate.AddUniqueDynamic(this, &UBaseLineInventory::ItemUpdate);
}

void UBaseLineInventory::UnbindItem(UBaseItem* ItemRef)
{
	if (!(ItemRef && ItemRef->OnItemUpdate.IsAlreadyBound(this, &UBaseLineInventory::ItemUpdate))) return;

	ItemRef->OnItemUpdate.RemoveDynamic(this, &UBaseLineInventory::ItemUpdate);
}

void UBaseLineInventory::ItemUpdate(UBaseItem* ItemRef)
{
	CalculateCurrentWeight();
	OnUpdateInventoryItems.Broadcast(this);
}

void UBaseLineInventory::CalculateCurrentWeight()
{
	if (!GetWorld()) return;
	if (GetWorld()->GetTimerManager().IsTimerActive(RecalculateWeight)) return;
	GetWorld()->GetTimerManager().SetTimer(RecalculateWeight, this, &UBaseLineInventory::UpdateWeight, 0.01f);
}

void UBaseLineInventory::UpdateWeight()
{
	InventoryWeight = 0;
	for (auto LItem : GetAllItem())
	{
		InventoryWeight += 0;// LItem->GetWeight();
	}
	OnWeightChange.Broadcast(InventoryWeight);
}

bool UBaseLineInventory::Contains(UBaseItem* ItemRef)
{
	return GetAllItem().Contains(ItemRef);
}
#pragma endregion TOOLS


FInventorySave::FInventorySave(TMap<UBaseItem*, FIntPoint> Data, AActor* Owner)
{
	SavedData.Empty();
	Saved = 1;
	TArray<AActor*> FilterOwner;
	FilterOwner.Add(Owner);
	if (Cast<APlayerController>(Owner))
		FilterOwner.Add(Cast<APlayerController>(Owner)->GetPawn());

	for (auto Item : Data)
	{
		if (!FilterOwner.Contains(Item.Key->GetOuter()))
		{
			if (APawn* LPawn = Cast<APawn>(Item.Key->GetOuter()))
			{
				if (!FilterOwner.Contains(LPawn->GetController())) continue;
			}
			else continue;
		}
		SavedData.Add(FInventoryItemSave(Item.Key));
	}
}

FInventorySave::FInventorySave(TMap<UBaseItem*, FIntPoint> Data, int32 ColumSize, AActor* Owner)
{
	SavedData.Empty();
	Saved = 1;
	TArray<AActor*> FilterOwner;
	FilterOwner.Add(Owner);
	if (Cast<APlayerController>(Owner))
		FilterOwner.Add(Cast<APlayerController>(Owner)->GetPawn());

	for (auto Item : Data)
	{
		if (!FilterOwner.Contains(Item.Key->GetOuter()))
		{
			if (APawn* LPawn = Cast<APawn>(Item.Key->GetOuter()))
			{
				if (!FilterOwner.Contains(LPawn->GetController())) continue;
			}
			else continue;
		}

		SavedData.Add(FInventoryItemSave(Item.Key));
	}
}

FInventoryItemSave::FInventoryItemSave(UBaseItem* Item)
{
	if (!IsValid(Item)) return;

	FMemoryWriter MemoryWriter(ItemData, true);
	FSaveGameArchive ArSave(MemoryWriter);
	Item->Serialize(ArSave);
	SavedClass = Item->GetClass();
}