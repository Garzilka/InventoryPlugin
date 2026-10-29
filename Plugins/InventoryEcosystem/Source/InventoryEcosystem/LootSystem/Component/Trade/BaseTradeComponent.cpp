// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTradeComponent.h"

#include "Engine/World.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"


UBaseTradeComponent::UBaseTradeComponent(){}


FCurrencysRepData UBaseTradeComponent::GetTradeCurrency(APawn* Instigator)
{
	FCurrencysRepData Result;
	if (CurrencyData.CurrencyValue.Num() == 0)
	{
		FCurrencyData LCurrency;		
		LCurrency.CyrrencyTag = NAME_None;
		LCurrency.SetValue(0);
		Result.CurrencyData.Add(LCurrency);
		return Result;
	}
	if (CurrencyData.CurrencyValue[0].CurrencyValues.Num() == 0)
	{
		FCurrencyData LCurrency;
		
		LCurrency.CyrrencyTag = NAME_None;
		LCurrency.SetValue(0);
		Result.CurrencyData.Add(LCurrency);
		return Result;
	}
	
	int32 LCurrentScore = GetScore(Instigator);
	int32 Min = 0;
	int32 Max = 0;
	
	for (int32 Idx = 0; Idx < CurrencyData.CurrencyValue.Num(); Idx++)
	{
		auto& LCurrencySettings = CurrencyData.CurrencyValue[Idx];
		Max = LCurrencySettings.Score;
		
		if (LCurrentScore > Min && LCurrentScore <= Max)
		{
			Result = FCurrencysRepData();
			for (auto LCurrencyData : LCurrencySettings.CurrencyValues)
			{
				FCurrencyData LCurrency; 
				LCurrency.CyrrencyTag = LCurrencyData.CurrencyType.CurrencyType;
				LCurrency.SetValue(LCurrencyData.Value);
				Result.CurrencyData.Add(LCurrency);
			}
			return Result;
		}
	
		Min = Max;
		Idx++;
	
		if (CurrencyData.CurrencyValue.Num() == Idx && LCurrentScore > 0)
		{
			Result = FCurrencysRepData();
			for (auto LCurrencyData : LCurrencySettings.CurrencyValues)
			{
				FCurrencyData LCurrency; 
				LCurrency.CyrrencyTag = LCurrencyData.CurrencyType.CurrencyType;
				LCurrency.SetValue(LCurrencyData.Value);
				Result.CurrencyData.Add(LCurrency);
			}
			return Result;
		}
	}
	
	return Result;
}

void UBaseTradeComponent::BeginPlay()
{
	Super::BeginPlay();
}
