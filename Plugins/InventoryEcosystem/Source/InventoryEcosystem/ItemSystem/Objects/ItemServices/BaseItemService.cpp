// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItemService.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"

void UBaseItemService::BeginPlay(UBaseItem* Item)
{
	ItemOwner = Item;
}

UBaseItem* UBaseItemService::GetItemOwner()
{
	if (!IsValid(ItemOwner))
	{
		ItemOwner = Cast<UBaseItem>(GetOuter());
	}
	return ItemOwner;
}

AActor* UBaseItemService::GetOwner()
{
	if (GetItemOwner())
	{
		return GetItemOwner()->GetOwner();
	}
	return Super::GetOwner();
}

void UBaseItemService::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
