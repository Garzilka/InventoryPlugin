// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTradeService.h"

#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"

TMap<FName, int32> UBaseTradeService::GetCost(FName TradeType)
{
	TMap<FName, int32> Result;
	for (auto& Pair : CostByCurrency.CurrenciesData)
	{
		if (Pair.TradeType.TradeType == TradeType)
		{
			for (auto CurrencyData : Pair.CurrencyValue)
			{
				Result.Add(CurrencyData.CurrencyType.CurrencyType, (CurrencyData.Value * GetItemOwner()->GetAmount()));
			}
		}
	}
	return Result;
}

int32 UBaseTradeService::GetCostByCurrencyType(FName TradeType, FName Tag)
{
	for (auto& Pair : CostByCurrency.CurrenciesData)
	{
		if (Pair.TradeType.TradeType != TradeType) continue;
		
		for (auto CurrencyData : Pair.CurrencyValue)
		{
			if (CurrencyData.CurrencyType.CurrencyType != Tag) continue;
			
			return (CurrencyData.Value * GetItemOwner()->GetAmount());
		}
	}
	return 0;
}
