// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItemService.h"
#include "BasePickupService.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ITEMSYSTEM_API UBasePickupService : public UBaseItemService
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent)
	void PickupItem(APawn* Instigator);
	void PickupItem_Implementation(APawn* Instigator) {};
};
