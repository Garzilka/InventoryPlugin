// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "MakeItemRule.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ITEMSYSTEM_API UMakeItemRule : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBaseItem> ItemClass;

public:

	TSubclassOf<UBaseItem> GetItemClass() {return ItemClass;};

	UFUNCTION(BlueprintImplementableEvent)
	void MakeItem(UBaseItem* Item);
	void MakeItem_Implementation(UBaseItem* Item) {};
};
