// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CurrencyData.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseLineInventory.h"
#include "TradeData.generated.h"


class UBaseTradeManager;
class UBaseTradeComponent;

UENUM(BlueprintType)
enum class ETradeType : uint8
{
	ETradeType_None				UMETA(DisplayName = "None"),
	ETradeType_P_TO_P			UMETA(DisplayName = "Player To Player"),
	ETradeType_P_TO_NPC			UMETA(DisplayName = "Player To NPC"),
	ETradeType_P_TO_Stash		UMETA(DisplayName = "Player To Stash"),
	ETradeType_P_TO_Container	UMETA(DisplayName = "Player To Container"),
};

USTRUCT(BlueprintType)
struct FTradeModificate
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName Tag = FName();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Модификатор покупки"))
	float ModPlayerBuy = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Модификатор продажи"))
	float ModPlayerSell = 0.5f;
};


USTRUCT(BlueprintType)
struct FTradeState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	ETradeType CurrentTrade = ETradeType::ETradeType_None;

	UPROPERTY(BlueprintReadOnly)
	FName CurrentTradeType = NAME_None;
	
	UPROPERTY(BlueprintReadOnly)
	FCurrencysRepData TraderCurrencyData;

	UPROPERTY(BlueprintReadOnly)
	bool bConfirmationSelf = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bConfirmationAnother = false;

	UPROPERTY(BlueprintReadOnly)
	UBaseLineInventory* AnotherInventory;

	UPROPERTY()
	UBaseTradeManager* AnotherTradeManager;

	UPROPERTY()
	UBaseTradeComponent* AnotherTradeComponent;

	void Reset()
	{
		CurrentTrade = ETradeType::ETradeType_None;
		bConfirmationSelf = false;
		bConfirmationAnother = false;
		AnotherInventory = nullptr;
		AnotherTradeManager = nullptr;
		AnotherTradeComponent = nullptr;
	}

	void DestroyTemp()
	{
		if (IsValid(AnotherInventory))
		{
			AnotherInventory->ClearInventory();
			AnotherInventory->DestroyComponent();
			AnotherInventory->UnregisterComponent();
			AnotherInventory = nullptr;
		}
	}
};

USTRUCT(BlueprintType)
struct FCurrencyCalculation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FName CurrencyType;

	void Reset() 
	{
		Income = 0;
		Expenses = 0;
	}

	void SetValue(int32 InIncome, int32 InExpenses, TArray<FTradeModificate> TradeGoldModificators)
	{
		float ResultToAdd = 1.f;
		float ResultToSub = 1.f;
		for (FTradeModificate& TradeMod : TradeGoldModificators)
		{
			ResultToAdd *= TradeMod.ModPlayerSell;
			ResultToSub *= TradeMod.ModPlayerBuy;
		}
		Income = InIncome * ResultToAdd;
		Expenses = InExpenses * ResultToSub;
		if ((InIncome * ResultToAdd) > 0.f && Income == 0)
		{
			Income = 1;
		}
		if ((InExpenses * ResultToSub) > 0.f && Expenses == 0)
		{
			Expenses = 1;
		}
	}

	int32 GetIncome() const
	{
		return Income;
	}

	int32 GetExpenses() const
	{
		return Expenses;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 Income = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Expenses = 0;
};

USTRUCT(BlueprintType)
struct FCurrenciesCalculation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FCurrencyCalculation> CurrencyData;

	FCurrencyCalculation& FindOrAdd(FName ByTag)
	{
		for (FCurrencyCalculation& LCurrencyData : CurrencyData)
		{
			if (LCurrencyData.CurrencyType != ByTag) continue;
			return LCurrencyData;
		}
		FCurrencyCalculation LAddCurrencyData;
		LAddCurrencyData.CurrencyType = ByTag;
		return CurrencyData.Add_GetRef(LAddCurrencyData);
	}
};

UCLASS()
class LOOTSYSTEM_API UTradeData : public UObject
{
	GENERATED_BODY()
	
};
