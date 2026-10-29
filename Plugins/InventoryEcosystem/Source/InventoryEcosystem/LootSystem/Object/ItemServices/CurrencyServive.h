// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "CurrencyServive.generated.h"

/**
 * 
 */
UCLASS()
class LOOTSYSTEM_API UCurrencyServive : public UBaseItemService
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere, Category="Торговля")
	FCurrencyType CyrrencyType;
	
	UPROPERTY(EditAnywhere, Category="Торговля")
	int32 NominalValue = 100;

public:
	UFUNCTION(BlueprintPure)
	FCurrencyType GetCurrencyType() const { return CyrrencyType; };
	
	UFUNCTION(BlueprintPure)
	int32 GetNominalValue() const { return NominalValue; };

	
};
