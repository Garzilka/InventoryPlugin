// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGridInventory.h"


UBaseGridInventory::UBaseGridInventory() 
{
	SetIsReplicatedByDefault(true);
	AsGrid = true;
}

UBaseGridInventory* UBaseGridInventory::GetGridInventory(AActor* FromActor)
{
	if (!IsValid(FromActor)) return nullptr;
	return Cast<UBaseGridInventory>(FromActor->GetComponentByClass(UBaseGridInventory::StaticClass()));
}

void UBaseGridInventory::BeginPlay()
{
	Super::BeginPlay();
}

void UBaseGridInventory::InventoryUpdate()
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

void UBaseGridInventory::ReCalculateInventory()
{
}

void UBaseGridInventory::SaveData()
{
	SavedData = FInventorySave(GetAllItemGrid(), GetColums(), GetOwner());
	SavedData.Size = InventoryData.GetSize();
}