// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "NativeGameplayTags.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/LootSystem/Object/Generate/ScoreGenerateRule.h"

#include "LootSystemData.generated.h"

UENUM(BlueprintType)
enum class EGenerateType : uint8
{
    EGIT_Probable 			UMETA(DisplayName = "Вероятный"),
    EGIT_Random				UMETA(DisplayName = "Рандомный"),
    EGIT_Progressive			UMETA(DisplayName = "Прогрессивный"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EGenerateType, EGenerateType::EGIT_Probable, EGenerateType::EGIT_Progressive);

UENUM(BlueprintType)
enum class EProgressiveGenType : uint8
{
    EPGT_Random			UMETA(DisplayName = "Рандомный"),
    EPGT_Probable 		UMETA(DisplayName = "Вероятный"),
};

UENUM(BlueprintType)
enum class ETypeCheckClass : uint8
{
    EGRT_ExcludeItem			UMETA(DisplayName = "Исключать дубликаты"),
    EGRT_IncludeAndSumm			UMETA(DisplayName = "Суммировать при возможности"),
    EGRT_Ignore				UMETA(DisplayName = "Игнорировать дубликаты"),
};

UENUM(BlueprintType)
enum class ETypeOfQuantityProcessing : uint8
{
    ETOQP_ByPassage 		UMETA(DisplayName = "Требуемое количетсво проходов"),
    ETOQP_ByItem		UMETA(DisplayName = "Требуемое количетсво предметов"),
};

UENUM(BlueprintType)
enum class EGenerationLootType : uint8
{
    EGLT_None			UMETA(DisplayName = "None"),
    EGLT_OnlyOne		UMETA(DisplayName = "Разовый спавн"),
    EGLT_BeginPlay		UMETA(DisplayName = "На старт игры"),
    EGLT_Timer			UMETA(DisplayName = "По таймеру"),
    EGLT_TimerAfterPickup	UMETA(DisplayName = "По таймеру если был затронут"),
    EGLT_DailyTime		UMETA(DisplayName = "Каждый день"),
};

UENUM(BlueprintType)
enum class EBlockEnableType : uint8
{
    EBET_Enabled		UMETA(DisplayName = "Включен"),
    EBET_EnableIf		UMETA(DisplayName = "Включить если"),
    EBET_DisableIf		UMETA(DisplayName = "Выключить если"),
};

UENUM(BlueprintType)
enum class EItemAmoutModificate : uint8
{
    EGWB_None			UMETA(Hidden),
    EIAB_Add			UMETA(DisplayName = "Добавить"),
    EIAB_Substruct		UMETA(DisplayName = "Вычесть"),
    EIAB_AddPercent		UMETA(DisplayName = "Добавить процент"),
    EIAB_SubstructPercent	UMETA(DisplayName = "Вычесть процент"),
};

#pragma region TOOLS_AMOUNT_GENERATE

USTRUCT(BlueprintType)
struct FObjectMetaData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Категория объекта"))
	FName Category = FName("Unsorted");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Имя объекта"))
	FText Object_Name;

	UPROPERTY()
	bool Collapsed = true;
};

USTRUCT(BlueprintType)
struct FRespawnLootData
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 0, DisplayName = "Тип респавна"))
	EGenerationLootType RespawnLootType = EGenerationLootType::EGLT_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Generate, meta = (DisplayName = "Время рестарта", UIMin = 0, ClampMin = 0, UIMax = 86400, ClampMax = 604800, DisplayPriority = 0))
	float TimeToRestart = 600.f;
};

USTRUCT(BlueprintType)
struct FAmountGenerateSettings
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Мин"))
	int32 Min = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Макс"))
	int32 Max = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Тип проходов"))
	ETypeOfQuantityProcessing TypeOfGenerate = ETypeOfQuantityProcessing::ETOQP_ByPassage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Тип проверки дубликатов"))
	ETypeCheckClass TypeTypeCheckClass = ETypeCheckClass::EGRT_IncludeAndSumm;
};

#pragma endregion TOOLS_AMOUNT_GENERATE 

#pragma region DATA_LOOT_ROWS

USTRUCT(BlueprintType)
struct FRandomItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Строка за строкой"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMakeItemContext GenerateCell;
};

USTRUCT(BlueprintType)
struct FRandomItemsData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRandomItemData> RandomItemsData;
};

USTRUCT(BlueprintType)
struct FLootRowData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FObjectMetaData MetaData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 1, DisplayName = "Рандомная генерация"))
	FRandomItemsData RandomGeneration;
};

#pragma endregion DATA_LOOT_ROWS

#pragma region SETTINGS_LOOT_RANDOM

USTRUCT(BlueprintType)
struct FGenerateDataRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Строка: "))
	FName Row;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Вес: "))
	float Weight = 1.f;
};

USTRUCT(BlueprintType)
struct FRandomRowsData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Строки генерации"))
	TArray<FGenerateDataRow> Rows;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Генерация количества"))
	FAmountGenerateSettings AmountGenerateSettings;
};

#pragma endregion SETTINGS_LOOT_RANDOM

#pragma region SETTINGS_LOOT_PROBABLE

USTRUCT(BlueprintType)
struct FProbableGenerationCell
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta =
		(DisplayName = "Строка за строкой", ClampMax=100, ClampMin=0, UIMax=100, UIMin=0, ForceUnits = "%"))
	float Percent = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMakeItemContext GenerateCell;
};

USTRUCT(BlueprintType)
struct FProbableGenerationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FProbableGenerationCell> GenerateCells;
};

#pragma endregion SETTINGS_LOOT_PROBABLE

#pragma region SETTINGS_LOOT_PROGRESSIVE 



USTRUCT(BlueprintType)
struct FRandomItemsDataAmount
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRandomItemData> RandomRowsData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Генерация количества"))
	FAmountGenerateSettings AmountGenerateSettings;

	bool IsValidGenData();
};

USTRUCT(BlueprintType)
struct FProgressiveDataCell
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Категория лута"))
	FString CategoryName;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Тип генерации"))
	EProgressiveGenType TypeGeneration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Рандомная генерация"))
	FRandomItemsDataAmount RandomGenerationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Вероятная генерация"))
	FProbableGenerationSettings ProbableGenerationSettings;
};


USTRUCT(BlueprintType)
struct FProgressiveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Настройка появления рандомных предметов", TitleProperty = "Name"))
	TArray<FProgressiveDataCell> ProgressiveDataCells;
};

USTRUCT(BlueprintType)
struct FProgressiveGenerationSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Настройки"))
	TMap<int32, FProgressiveData> ProgressiveData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayPriority = 1, DisplayName = "Правила генерации"))
	TSubclassOf<UScoreGenerateRule> GenerateRules = {};
};

#pragma endregion SETTINGS_LOOT_PROGRESSIVE


USTRUCT(BlueprintType)
struct FGenerateSettingsBlocks
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Тип работы"))
	EBlockEnableType EnableSetting = EBlockEnableType::EBET_Enabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Тэг лута"))
	FName LootTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 1, DisplayName = "Тип генерации"))
	EGenerateType GenerateType = EGenerateType::EGIT_Random;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 1, DisplayName = "Вероятная генерация"))
	FProbableGenerationSettings ProbableGenerationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 1, DisplayName = "Вероятная генерация"))
	FRandomRowsData RandomGenerateSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = 1, DisplayName = "Прогрессивная генерации"))
	FProgressiveGenerationSettings ProgressiveGenerationSettings;
};

USTRUCT(BlueprintType)
struct FGenerateSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseRespawnView = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Уникальный лут", DisplayPriority = 1))
	bool bIsUniqueLoot = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRespawnLootData RespawnSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Блоки генерации"))
	TArray<FGenerateSettingsBlocks> GenerateBlocks;
};

USTRUCT(BlueprintType)
struct FLootPressetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FObjectMetaData MetaData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGenerateSettings GenerateSettings;	
};

USTRUCT(BlueprintType)
struct FLootSettingsData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FObjectMetaData MetaData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUsePreset = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PresetGUID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGenerateSettings GenerateSettings;
};

DECLARE_DELEGATE(FUpdateLootDataSignature)

UCLASS()
class LOOTSYSTEM_API ULootData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

    FUpdateLootDataSignature OnUpdateLootSettingData;
    
	bool AddLootObject(FName GUID, FName Name)
	{
	    if (LootSettings.Contains(GUID)) return false;
		
	    FLootSettingsData LootObject;
	    LootObject.MetaData.Object_Name = FText::FromName(Name);
	    LootSettings.Add(GUID, LootObject);
	    Modify(true);
	    OnUpdateLootSettingData.ExecuteIfBound();
	    return true;
	}
	
	ULootData()	{};

	TMap<FName, FLootSettingsData>& GetLootSettingsData() {return LootSettings;};
	TMap<FName, FLootRowData>& GetLootData() {return LootData;};
	TMap<FName, FLootPressetData>& GetLootPresets() {return LootPresets;};
public:
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FLootSettingsData> LootSettings;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FLootPressetData> LootPresets;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FLootRowData> LootData;
};