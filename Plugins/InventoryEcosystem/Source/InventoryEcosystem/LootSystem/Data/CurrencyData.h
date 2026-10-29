// Copyright (c) 2022 GFA Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "UObject/NoExportTypes.h"
#include "CurrencyData.generated.h"

USTRUCT(BlueprintType)
struct FCurrencyType
{
	GENERATED_BODY()

	FCurrencyType() = default;
	FCurrencyType(FName InName) { CurrencyType = InName;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrencyType;
};

USTRUCT(BlueprintType)
struct FTradeType
{
	GENERATED_BODY()

	FTradeType() = default;
	FTradeType(FName InName) { TradeType = InName;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TradeType = NAME_None;	
};

USTRUCT(BlueprintType)
struct FCurrencyValue
{
	GENERATED_BODY()

	FCurrencyValue() = default;
	FCurrencyValue(FName InCurrencyType, int32 InValue) {CurrencyType.CurrencyType = InCurrencyType; Value = InValue;};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCurrencyType CurrencyType;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;	
};

USTRUCT(BlueprintType)
struct FCurrencyValueByScore
{
	GENERATED_BODY()

	FCurrencyValueByScore() = default;
	FCurrencyValueByScore(int32 InScore, TArray<FCurrencyValue> InCurrencyValues) {Score = InScore; CurrencyValues = InCurrencyValues;};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsExpended = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Score;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrencyValue> CurrencyValues;
};

USTRUCT(BlueprintType)
struct FCurrencyDataByTradeType
{
	GENERATED_BODY()

	FCurrencyDataByTradeType() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsExpended = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTradeType TradeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrencyValue> CurrencyValue;
};

USTRUCT(BlueprintType)
struct FCurrencyTraderData
{
	GENERATED_BODY()

	FCurrencyTraderData() = default;
	FCurrencyTraderData(FName InTradeType) {TradeType = FTradeType(InTradeType);};

	bool IsValidSettings() const { return (TradeType.TradeType != NAME_None && CurrencyValue.Num() > 0); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsExpended = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTradeType TradeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrencyValueByScore> CurrencyValue;
};

USTRUCT(BlueprintType)
struct FItemCurrecyTradeData
{
	GENERATED_BODY()

	FItemCurrecyTradeData() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsExpended = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCurrencyDataByTradeType> CurrenciesData;
};

USTRUCT(BlueprintType)
struct FCurrencyData
{
	GENERATED_BODY()

public:
	FCurrencyData() = default;
	
	UPROPERTY(BlueprintReadOnly)
	FName CyrrencyTag;

	void SetValue(int32 NewValue)
	{
		OldValue = Value;
		Value = NewValue;		
	}

	int32 GetValue() const
	{
		return Value;		
	}

	int32 GetOldValue() const
	{
		return OldValue;		
	}

	friend bool operator==(const FCurrencyData& Left, const FCurrencyData& Right)
	{
		return Left.CyrrencyTag == Right.CyrrencyTag;
	}

	friend bool operator==(const FCurrencyData& Left, const FName& Right)
	{
		return Left.CyrrencyTag == Right;
	}

protected:

	UPROPERTY(BlueprintReadOnly)
	int32 Value = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 OldValue = 0;
};

USTRUCT(BlueprintType)
struct FCurrencysRepData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FCurrencyData> CurrencyData;

	FCurrencyData& FindOrAdd(FName ByTag)
	{
		for (FCurrencyData& LCurrencyData : CurrencyData)
		{
			if (LCurrencyData.CyrrencyTag != ByTag) continue;
			return LCurrencyData;
		}
		FCurrencyData LAddCurrencyData;
		LAddCurrencyData.CyrrencyTag = ByTag;
		return CurrencyData.Add_GetRef(LAddCurrencyData);
	}
};

UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
	EPI_None			UMETA(Hidden),
	ECT_Value			UMETA(DisplayName = "Значение"),
	ECT_Item			UMETA(DisplayName = "Предмет в инвентаре"),	
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECurrencyView : uint8
{
	ECV_None			= 0					UMETA(Hidden),
	ECV_InventoryWindow	= 1 << 0			UMETA(DisplayName = "Инвентарь"),
	ECV_ItemCardWindow	= 1 << 1			UMETA(DisplayName = "Карточка предмета"),
	ECV_TradeWindow		= 1 << 2			UMETA(DisplayName = "Окно торговли"),
	ECV_CraftWindow		= 1 << 3			UMETA(DisplayName = "Окно крафта"),
};

UENUM(BlueprintType)
enum class EItemCardCurrencyView : uint8
{
	EICCV_Always		UMETA(DisplayName = "Всегда"),
	EICCV_Separate		UMETA(DisplayName = "Только при типе торговле"),
};

UENUM(BlueprintType)
enum class ETradeWindowsCurrencyView : uint8
{
	ETWCV_Always		UMETA(DisplayName = "Всегда"),
	ETWCV_Separate		UMETA(DisplayName = "Только при типе торговле"),
};

USTRUCT(BlueprintType)
struct FCurrencySettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Активна ли валюта", ToolTip = "Если выключен, тогда не участвует в торговле и не отображается"))
	bool bIsActive = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/LootSystem.ECurrencyView", EditCondition = "bIsActive", DisplayName = "Отображение валюты"))
	int32 BitMaskWindowView;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsActive && BitMaskWindowView & ECurrencyView::ECV_ItemCardWindow", EditConditionHides, DisplayName = "Отображение в карточке"))
	EItemCardCurrencyView ItemCardCurrencyView = EItemCardCurrencyView::EICCV_Always;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsActive && BitMaskWindowView & ECurrencyView::ECV_TradeWindow", EditConditionHides, DisplayName = "Отображение в торговле"))
	ETradeWindowsCurrencyView TradeWindowCurrencyView = ETradeWindowsCurrencyView::ETWCV_Always;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Условие типа торговли", EditConditionHides,
		EditCondition = "TradeWindowCurrencyView == ETradeWindowsCurrencyView::ETWCV_Separate || ItemCardCurrencyView == EItemCardCurrencyView::EICCV_Separate"))
	FTradeType ViewByTradeType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bIsActive", DisplayName = "Название валюты"))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bIsActive", DisplayName = "Иконка валюты"))
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bIsActive", DisplayName = "Тип валюты", ToolTip = "Будет ли предмет распыляться в значение, или будет храниться в инвентаре"))
	ECurrencyType CurrencyType = ECurrencyType::ECT_Value;
};



UCLASS()
class LOOTSYSTEM_API UTradeSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Настройки валют", ForceInlineRow))
	TMap<FName, FCurrencySettings> CurrencySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Настройки типов торговли"))
	TArray<FName> TradeTypes;
	
	UFUNCTION(BlueprintPure)
	ECurrencyType GetCurrencyTypeFor(UPARAM(Meta = (CurrencyTypes)) FName InTag)
	{
		if (auto Data = CurrencySettings.Find(InTag))
		{
			return Data->CurrencyType;
		}
		return ECurrencyType::EPI_None;
	}
	
};