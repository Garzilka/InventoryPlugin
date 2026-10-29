// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "BaseTradeService.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class LOOTSYSTEM_API UBaseTradeService : public UBaseItemService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Торговля", meta = (DisplayName = "Цена по типу торговли"))
	FItemCurrecyTradeData CostByCurrency;

public:

	UFUNCTION(BlueprintPure, Category = "Setting | Item")
	TMap<FName, int32> GetCost(UPARAM(Meta = (TradeTypes)) FName TradeType);

	UFUNCTION(BlueprintPure, Category = "Setting | Item")
	int32 GetCostByCurrencyType(UPARAM(Meta = (TradeTypes)) FName TradeType, UPARAM(Meta = (CurrencyTypes)) FName Tag);
};
