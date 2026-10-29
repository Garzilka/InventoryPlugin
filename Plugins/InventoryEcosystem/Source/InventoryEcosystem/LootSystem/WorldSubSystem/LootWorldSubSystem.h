// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "LootWorldSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class LOOTSYSTEM_API ULootWorldSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	TArray<UBaseItem*> GenerateItems(const FGenerateSettings& Settings, AActor* InItemOwner);
	UBaseItem* GenerateItem(const FGenerateSettings& Settings, AActor* InItemOwner);
	TArray<FMakeItemContext> GenerateItemsData(const FGenerateSettings& Settings, AActor* InItemOwner);

	
private:
	TArray<FMakeItemContext> GenerateItemData(const FGenerateSettingsBlocks& GenerationSettings);
	TArray<FMakeItemContext> GenerateItemData(const FProbableGenerationSettings& GenerationSettings);
	TArray<FMakeItemContext> GenerateItemData(const FRandomRowsData& GenerationSettings);
	TArray<FMakeItemContext> GenerateItemData(const FRandomItemsDataAmount& GenerationSettings);
	TArray<FMakeItemContext> GenerateItemData(const FProgressiveGenerationSettings& GenerationSettings);
	// TArray<FMakeItemContext> GenerateItemData(UProgressiveGeneration* GenerationSettings, UScoreGenerateRule* GenerateRules);
	// TArray<FMakeItemContext> GenItemsData(TArray<FSpawnCell> Data, int32 CountForGroup, int32 MaxCount, ETypeCheckClass LimitType, ETypeOfQuantityProcessing TypeOfQuantity, TArray<TSubclassOf<UBaseItem>> IgnoreList);
	// void RecalculateWeightMax(int32& Max, float& Possible, TArray<FSpawnCell> Array);
	//
	// TArray<FMakeItemContext> GenerateItemData(FRandomGenetateSettings Data);
	FGenerateDataRow GenerateRow(const TArray<FGenerateDataRow>& RowData);
	TArray<FMakeItemContext> GenerateItemData(const FGenerateDataRow& RowData);
	TArray<FMakeItemContext> GenerateItemData(const TArray<FRandomItemData>& RowData);
	void GenerateAmount(const FAmountGenerateSettings& AmountGenerateSettings);
	//
	// void RefreshLootTags();
	AActor* ItemOwner = nullptr;;
	// TArray<APlayerController*> CurrentOwners;
	// FGameplayTagContainer CurrentLootTags;
	// FGenerateData CurrentGenerateData;
	//
	// bool IsRowActive(const FGenerateRowData& RowData) const;
	// void UpdateRowWeight(FGenerateRowData& RowData) const;
	// void ModificateItemByRow(FGenerateRowData& RowData, FMakeItemContext& ItemData);
	//
	ETypeOfQuantityProcessing TypeOfGenerate = ETypeOfQuantityProcessing::ETOQP_ByPassage;
	ETypeCheckClass TypeTypeCheckClass = ETypeCheckClass::EGRT_Ignore;
	int32 TargetAmountItems = -1;
	//
	//
	//
	void GenerateItemsData(TArray<FGenerateDataRow> Data);
	TArray<FRandomItemData> CurrentItemsData;
};
