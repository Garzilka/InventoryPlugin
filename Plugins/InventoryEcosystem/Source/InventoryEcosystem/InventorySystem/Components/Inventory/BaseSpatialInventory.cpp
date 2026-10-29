// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSpatialInventory.h"


// Sets default values for this component's properties
UBaseSpatialInventory::UBaseSpatialInventory()
{
	SetIsReplicatedByDefault(true);
	AsGrid = true;
}


// Called when the game starts
void UBaseSpatialInventory::BeginPlay()
{
	Super::BeginPlay();
	AsGrid = true;
}

bool UBaseSpatialInventory::IsPositionAvailable(UBaseItem* ItemRef, int32 Index, bool IsNeedStackItems)
{
	if (!IsValid(ItemRef))	return false;

	FIntPoint LTitle = ConvertIndexToPosition(Index);
	FIntPoint LDimension = ItemRef->GetDimensions();

	for (int32 i = LTitle.X; i < LTitle.X + LDimension.X; i++)
	{
		for (int32 j = LTitle.Y; j < LTitle.Y + LDimension.Y; j++)
		{
			FIntPoint CurrentPosition = FIntPoint(i, j);
			if (!IsPositionValid(CurrentPosition)) return false;

			UBaseItem* LItem = GetItemAtIndex(ConvertPositionToIndex(CurrentPosition));
			if (!LItem) continue;

			if (LItem == ItemRef) continue; //Collision with self

			if (!IsNeedStackItems)	return false;

			if (!LItem->IsStackable())
			{
				return false;
			}

			if (LItem->IsStackable() && LItem->GetClass() != ItemRef->GetClass())
			{
				return false;
			}

		}
	}

	return true;
}

bool UBaseSpatialInventory::AddItemAt(UBaseItem* ItemRef, int32 TopLeftIndex, bool IsNeedStackItem)
{
	if (!(IsValid(ItemRef) && ItemRef->IsCanRemoveFromParrent())) return false;

	if (!IsPositionAvailable(ItemRef, TopLeftIndex, IsNeedStackItem)) return false;

	if (HasAuthority())
	{
		if (GetItemAtIndex(TopLeftIndex) && GetItemAtIndex(TopLeftIndex)->StackItems(ItemRef))
		{
			if (!IsValid(ItemRef))
			{
				return true;
			}
		}

		FIntPoint* Result = GetAllItemGrid().Find(ItemRef);
		ItemRef->ChangeOwner(GetOwner());

		if (Result)
		{
			return MoveItem(ConvertPositionToIndex(*Result), TopLeftIndex);
		}

		FIntPoint LTitle = ConvertIndexToPosition(TopLeftIndex);
		FIntPoint LDimension = ItemRef->GetDimensions();
		int32 Idx = 0;
		int32 LStackAmout = 0;
		int32 LRemaindAmount = 0;

		for (int32 i = LTitle.X; i < LTitle.X + LDimension.X; i++)
		{
			for (int32 j = LTitle.Y; j < LTitle.Y + LDimension.Y; j++)
			{
				Idx = ConvertPositionToIndex(FIntPoint(i, j));
				if (!(InventoryData.Inventory.IsValidIndex(Idx))) return false;


				if (GetItemAtIndex(Idx))
				{
					if (GetItemAtIndex(Idx)->GetClass() != ItemRef->GetClass()) continue;

					LRemaindAmount = GetItemAtIndex(Idx)->SetAmount(ItemRef->GetAmount() + GetItemAtIndex(Idx)->GetAmount());

					if (LRemaindAmount > 0)
					{
						ItemRef->SetAmount(LRemaindAmount);
						return false;
					}
				}
				else
				{
					InventoryData.Inventory[Idx] = ItemRef;
				}
			}
		}

		ItemRef->SwitchParrent(this);
		ItemRef->SwitchParrent(this);
		OnRep_Inventory();
	}
	else
	{
		Server_AddItemAt(ItemRef, TopLeftIndex, IsNeedStackItem);
	}
	return true;
}

bool UBaseSpatialInventory::RemoveItemAt(int32 TopLeftIndex, bool WithDelegate)
{
	UBaseItem* LItem = GetItemAtIndex(TopLeftIndex);
	if (!LItem) return false;
	FIntPoint LPosition = ConvertIndexToPosition(TopLeftIndex);
	FIntPoint Dimension = LItem->GetDimensions();
	int32 Idx = 0;
	bool LWasRemoved = false;
	for (int32 i = LPosition.X; i < (LPosition.X + Dimension.X); i++)
	{
		for (int32 j = LPosition.Y; j < (LPosition.Y + Dimension.Y); j++)
		{
			Idx = ConvertPositionToIndex(FIntPoint(i, j));
			InventoryData.Inventory[Idx] = nullptr;
			LWasRemoved |= true;
		}
	}
	if (LWasRemoved && WithDelegate)
	{
		OnLeaveItem.Broadcast(this, LItem, true);
	}

	ItemCache.InvalidateCache();

	OnRep_Inventory();
	return LWasRemoved;
}

bool UBaseSpatialInventory::CanAddItem(UBaseItem* InItem, int32 Amount, bool WithStack)
{
	if (!IsValid(InItem)) return false;

	if (WithStack && InItem->IsStackable())
	{
		int32 Remains = (Amount > -1 ? Amount : InItem->GetAmount());
		for (auto Item : GetAllItemByClass(InItem->GetClass()))
		{
			if (!IsValid(Item)) continue;
			
			Remains -= Item->GetAmount();			
			if (Remains < 1) return true;
		}
	}
	
	for (int32 Idx = 0; Idx < InventoryData.GetNumBySize(); Idx++)
	{
		if (!IsPositionAvailable(InItem, Idx, (WithStack && InItem->IsStackable()))) continue;
		return true;
	}
	
	return false;
}
