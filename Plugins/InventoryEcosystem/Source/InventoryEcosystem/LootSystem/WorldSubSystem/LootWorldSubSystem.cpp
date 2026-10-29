// Fill out your copyright notice in the Description page of Project Settings.


#include "LootWorldSubSystem.h"

#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootManager.h"


#pragma region GENERATE_ITEMS

TArray<UBaseItem *> ULootWorldSubSystem::GenerateItems(const FGenerateSettings &Settings, AActor *InItemOwner)
{
    ItemOwner = InItemOwner;
    if (!ItemOwner) return {};
    
    TArray<FMakeItemContext> GeneratedSettings;
    for (const FGenerateSettingsBlocks& Block : Settings.GenerateBlocks)
    {
        GeneratedSettings.Append(GenerateItemData(Block));
    }
    TArray<UBaseItem*> Result;

    for (const FMakeItemContext Setting : GeneratedSettings)
    {
        for (auto Item : UInventoryLibrary::MakeItems(InItemOwner, Setting))
        {
            if (!IsValid(Item)) continue;
            Result.Add(Item);
        }
    }
    return Result;
}

UBaseItem* ULootWorldSubSystem::GenerateItem(const FGenerateSettings &Settings, AActor *InItemOwner)
{
    ItemOwner = InItemOwner;
    if (!ItemOwner) return nullptr;
    
    TArray<FMakeItemContext> GeneratedSettings;
    for (const FGenerateSettingsBlocks& Block : Settings.GenerateBlocks)
    {
        GeneratedSettings.Append(GenerateItemData(Block));
    }
    TArray<UBaseItem*> Result;

    for (const FMakeItemContext Setting : GeneratedSettings)
    {
        for (auto Item : UInventoryLibrary::MakeItems(InItemOwner, Setting))
        {
            if (!IsValid(Item)) continue;
            return Item;
        }
    }
    return nullptr;
}

TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemsData(const FGenerateSettings &Settings, AActor *InItemOwner)
{
    ItemOwner = InItemOwner;
    if (!ItemOwner) return {};
    
    TArray<FMakeItemContext> GeneratedSettings;
    for (const FGenerateSettingsBlocks& Block : Settings.GenerateBlocks)
    {
        GeneratedSettings.Append(GenerateItemData(Block));
    }
    return GeneratedSettings;
}
#pragma endregion GENERATE_ITEMS


TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FGenerateSettingsBlocks& GenerationSettings)
{
    if (GenerationSettings.EnableSetting != EBlockEnableType::EBET_Enabled) return {};

    if (GenerationSettings.GenerateType == EGenerateType::EGIT_Probable)
    {
        return GenerateItemData(GenerationSettings.ProbableGenerationSettings);
    }

    if (GenerationSettings.GenerateType == EGenerateType::EGIT_Progressive)
    {
        return GenerateItemData(GenerationSettings.ProgressiveGenerationSettings);
    }

    if (GenerationSettings.GenerateType == EGenerateType::EGIT_Random)
    {
        return GenerateItemData(GenerationSettings.RandomGenerateSettings);
    }
    return {};
}

#pragma region PROBABLE_GENERATE

TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FProbableGenerationSettings &GenerationSettings)
{
    TArray<FMakeItemContext> Result;
    for (const FProbableGenerationCell& Data : GenerationSettings.GenerateCells)
    {
        float Probability = FMath::RandRange(0, 100);
        if (Data.Percent > 0 && Probability < Data.Percent)
        {
            Result.Add(Data.GenerateCell);
        }
    }
    return Result;
}

#pragma endregion PROBABLE_GENERATE

#pragma region RANDOM_GENERATE

TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FRandomRowsData &GenerationSettings)
{
    TArray<FMakeItemContext> Result;    
    TArray<FGenerateDataRow> CopyRowData;
    
    for (const FGenerateDataRow& Row : GenerationSettings.Rows)
    {	
        // if (!IsRowActive(Row)) continue;
        CopyRowData.Add(Row);
		
        // UpdateRowWeight(Row);
    }
    if (CopyRowData.Num() == 0) return Result;
	
    //Защита от дурака
    if (GenerationSettings.AmountGenerateSettings.TypeTypeCheckClass == ETypeCheckClass::EGRT_ExcludeItem)
    {
        GenerateItemsData(CopyRowData);					
    }
	
    GenerateAmount(GenerationSettings.AmountGenerateSettings);
    TypeOfGenerate = GenerationSettings.AmountGenerateSettings.TypeOfGenerate;
    TypeTypeCheckClass = GenerationSettings.AmountGenerateSettings.TypeTypeCheckClass;
    int32 IDx = 0;
    while (IDx != TargetAmountItems)
    {
        FGenerateDataRow RowData = GenerateRow(CopyRowData);
        if (RowData.Row == NAME_None) break;
        
        for (const FMakeItemContext& Item : GenerateItemData(RowData))
        {
            // ModificateItemByRow(RowData, Item);

            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_Ignore)
            {
                Result.Add(Item);
            }
				
            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_ExcludeItem)
            {
                if (Result.Find(Item) == INDEX_NONE)
                {
                    Result.Add(Item);
						
                    CurrentItemsData.RemoveAll([&](FRandomItemData Cell)->bool
                    {
                            return (Cell.GenerateCell == Item);
                    });
						
                    if (CurrentItemsData.Num() == 0) return Result;
                }
					
            }
				
            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_IncludeAndSumm)
            {
                int32 LItemIdx = Result.Find(Item);
                if (Result.IsValidIndex(LItemIdx))
                {
                    Result[LItemIdx].ItemData.Amount += Item.ItemData.Amount;
                }
                else
                {
                    Result.Add(Item);
                }
            }
            IDx = (TypeOfGenerate == ETypeOfQuantityProcessing::ETOQP_ByPassage ? (IDx + 1) : Result.Num());
        }
			
        if (IDx >= TargetAmountItems) break;
    }
	
    return Result;
}

void ULootWorldSubSystem::GenerateItemsData(TArray<FGenerateDataRow> Rows)
{
    CurrentItemsData.Reset();
    auto LootData = ULootSystemSetting::Get()->LootData;

    for (auto RowData : Rows)
    {
        FLootRowData* ItemsInfo = LootData->GetLootData().Find(RowData.Row);
        if (!ItemsInfo) continue;

        for (auto ItemData : ItemsInfo->RandomGeneration.RandomItemsData)
        {
            CurrentItemsData.Add(ItemData);
        }
    }
}

FGenerateDataRow ULootWorldSubSystem::GenerateRow(const TArray<FGenerateDataRow>& RowData)
{
    int32 Max = 0;
    FGenerateDataRow RowWithMaxVariation;
    float Possible = 0.f;

    for (const FGenerateDataRow& Row : RowData)
    {
        Max += (Row.Weight * 1000.f);
        if (Possible <= Row.Weight)
        {
            Possible = Row.Weight;
            RowWithMaxVariation = Row;
        }
    }

    int32 LRand = FMath::RandRange(0, Max);
    float LResult = float(LRand) / 1000.f;
    float CurrentRangeMin = 0;

    for (const FGenerateDataRow& Row : RowData)
    {
        if (LResult > CurrentRangeMin && LResult < CurrentRangeMin + Row.Weight)
            return Row;
        CurrentRangeMin += Row.Weight;
    }
	
    return RowWithMaxVariation;
}

TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FGenerateDataRow& RowData)
{
    auto LootData = ULootSystemSetting::Get()->LootData;
    FLootRowData* ItemsInfo = LootData->GetLootData().Find(RowData.Row);
    if (!ItemsInfo) return {};

    int32 Max = 0;
    float Possible = 0.f;
    for (FRandomItemData ItemCell : ItemsInfo->RandomGeneration.RandomItemsData)
    {
        Max += (ItemCell.Weight * 1000);
        if (Possible < ItemCell.Weight)
        {
            Possible = ItemCell.Weight;
        }
    }

    int32 LRand = FMath::RandRange(0, Max);
    float LResult = float(LRand) / 1000.f;
    float CurrentRangeMin = 0;
    for (auto& ItemCell : ItemsInfo->RandomGeneration.RandomItemsData)
    {
        if (LResult > CurrentRangeMin && LResult < CurrentRangeMin + ItemCell.Weight)
        {
            return {ItemCell.GenerateCell};
        }

        CurrentRangeMin += ItemCell.Weight;
    }
    return {};
}

void ULootWorldSubSystem::GenerateAmount(const FAmountGenerateSettings& AmountGenerateSettings)
{
    int32 LMin = AmountGenerateSettings.Min;
    int32 LMax = AmountGenerateSettings.Max;

    // for (auto RowInfo : AmountGenerateSettings.AmountBoostSettings)
    // {
    // 	if (!CurrentLootTags.HasTag(RowInfo.LootTag)) continue;
    //
    // 	switch (RowInfo.WeightBonusType)
    // 	{
    // 		case (EGenerateWeightBonus::EGWB_Add) :
    // 		{
    // 			LMin += RowInfo.Min;
    // 			LMax += RowInfo.Max;
    // 			break;
    // 		}
    // 		case (EGenerateWeightBonus::EGWB_Multiply) :
    // 		{
    // 			LMin *= RowInfo.Min;
    // 			LMax *= RowInfo.Max;
    // 			break;
    // 		}
    // 		case (EGenerateWeightBonus::EGWB_Replace) :
    // 		{
    // 			LMin = RowInfo.Min;
    // 			LMax = RowInfo.Max;
    // 			break;
    // 		}
    // 		case (EGenerateWeightBonus::EGWB_Substruct) :
    // 		{
    // 			LMin -= RowInfo.Min;
    // 			LMax -= RowInfo.Max;
    // 			break;
    // 		}
    // 	default: ;
    // 	}
    // }
    TargetAmountItems = FMath::RandRange(LMin, LMax);
}

#pragma endregion RANDOM_GENERATE

TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FProgressiveGenerationSettings &GenerationSettings)
{
    TArray<FMakeItemContext> Result;
    
    UScoreGenerateRule* Rule = NewObject<UScoreGenerateRule>(ItemOwner, GenerationSettings.GenerateRules);    
    if (!Rule) return Result;
	
    int32 PlayerScore = Rule->GetPlayerScore(Cast<APlayerController>(Rule->GetOuter()));
    TArray<TSubclassOf<UBaseItem>> IgnoreList = Rule->GetIngoreListClass(Cast<APlayerController>(Rule->GetOuter()));
    if (PlayerScore < 0) return Result;

    int32 Min = 0;
    int32 Max = 0;

    for (auto ProgressiveBlock : GenerationSettings.ProgressiveData)
    {
        Min = Max;
        Max = ProgressiveBlock.Key;
        if (PlayerScore >= Min && PlayerScore <= Max)
        {
            for (const FProgressiveDataCell& PregressiveCategory : ProgressiveBlock.Value.ProgressiveDataCells)
            {
                if (PregressiveCategory.TypeGeneration == EProgressiveGenType::EPGT_Probable)
                {
                    Result.Append(GenerateItemData(PregressiveCategory.ProbableGenerationSettings));
                }

                if (PregressiveCategory.TypeGeneration == EProgressiveGenType::EPGT_Random)
                {
                    Result.Append(GenerateItemData(PregressiveCategory.RandomGenerationSettings));
                }                
            }
            
            break;
        }
    }
    
    return Result;
}


TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const FRandomItemsDataAmount &GenerationSettings)
{
    TArray<FMakeItemContext> Result;    
    TArray<FRandomItemData> CopyRowData;
    
    for (const FRandomItemData& Row : GenerationSettings.RandomRowsData)
    {	
        // if (!IsRowActive(Row)) continue;
        CopyRowData.Add(Row);
		
        // UpdateRowWeight(Row);
    }
    if (CopyRowData.Num() == 0) return Result;
	
    //Защита от дурака
    if (GenerationSettings.AmountGenerateSettings.TypeTypeCheckClass == ETypeCheckClass::EGRT_ExcludeItem)
    {
        CurrentItemsData = CopyRowData;					
    }
	
    GenerateAmount(GenerationSettings.AmountGenerateSettings);
    TypeOfGenerate = GenerationSettings.AmountGenerateSettings.TypeOfGenerate;
    TypeTypeCheckClass = GenerationSettings.AmountGenerateSettings.TypeTypeCheckClass;
    int32 IDx = 0;
    while (IDx != TargetAmountItems)
    {
        for (const FMakeItemContext& Item : GenerateItemData(CopyRowData))
        {
            // ModificateItemByRow(RowData, Item);

            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_Ignore)
            {
                Result.Add(Item);
            }
				
            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_ExcludeItem)
            {
                if (Result.Find(Item) == INDEX_NONE)
                {
                    Result.Add(Item);
						
                    CurrentItemsData.RemoveAll([&](FRandomItemData Cell)->bool
                    {
                            return (Cell.GenerateCell == Item);
                    });
						
                    if (CurrentItemsData.Num() == 0) return Result;
                }
					
            }
				
            if (TypeTypeCheckClass == ETypeCheckClass::EGRT_IncludeAndSumm)
            {
                int32 LItemIdx = Result.Find(Item);
                if (Result.IsValidIndex(LItemIdx))
                {
                    Result[LItemIdx].ItemData.Amount += Item.ItemData.Amount;
                }
                else
                {
                    Result.Add(Item);
                }
            }
            IDx = (TypeOfGenerate == ETypeOfQuantityProcessing::ETOQP_ByPassage ? (IDx + 1) : Result.Num());
        }
			
        if (IDx >= TargetAmountItems) break;
    }
	
    return Result;
}


TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemData(const TArray<FRandomItemData> &RowData)
{
    auto LootData = ULootSystemSetting::Get()->LootData;

    int32 Max = 0;
    float Possible = 0.f;
    for (FRandomItemData ItemCell : RowData)
    {
        Max += (ItemCell.Weight * 1000);
        if (Possible < ItemCell.Weight)
        {
            Possible = ItemCell.Weight;
        }
    }

    int32 LRand = FMath::RandRange(0, Max);
    float LResult = float(LRand) / 1000.f;
    float CurrentRangeMin = 0;
    for (const FRandomItemData& ItemCell : RowData)
    {
        if (LResult > CurrentRangeMin && LResult < CurrentRangeMin + ItemCell.Weight)
        {
            return {ItemCell.GenerateCell};
        }

        CurrentRangeMin += ItemCell.Weight;
    }
    return {};
}
//
//
// TArray<FMakeItemContext> ULootWorldSubSystem::GenerateItemsData(const FGenerateData& Settings, TArray<APlayerController*> For)
// {
// 	CurrentGenerateData = Settings;	
// 	CurrentOwners = For;	
// 	RefreshLootTags();
//
// 	if (Settings.bIsUniqueLoot && Settings.GenerateType == EGenerateType::EGIT_Progressive)
// 	{
// 		UScoreGenerateRule* Rule = NewObject<UScoreGenerateRule>(ItemOwner, Settings.GenerateRules);
// 		return GenerateItemData(Settings.GenerationSettings, Rule);
// 	}
//
// 	TArray<FMakeItemContext> Resutl;
// 	if (!Settings.bIsUniqueLoot || Settings.GenerateType == EGenerateType::EGIT_Random)
// 	{
// 		if (Settings.HasGenFlag(EGenerateItemsType::EGIT_Guaranteed))
// 		{
// 			Resutl.Append(GenerateItemData(Settings.GuaranteedSettings));
// 		}
// 		if (Settings.HasGenFlag(EGenerateItemsType::EGIT_Random))
// 		{
// 			Resutl.Append(GenerateItemData(Settings.RandomSettings));
// 		}
// 	}
// 	
// 	return Resutl;
// }
//
// #pragma region PSEUDORANDOM 
//

//
// TArray<FMakeItemContext> ULootWorldSubSystem::GenItemsData(TArray<FSpawnCell> InData, int32 CountForGroup, int32 MaxCount, ETypeCheckClass TypeCheck, ETypeOfQuantityProcessing TypeOfQuantity, TArray<TSubclassOf<UBaseItem>> IgnoreList)
// {
// 	if (MaxCount == 0) return {};
// 	TArray<FMakeItemContext> RandomItems;
//
// 	//В случае если требуется количество предметов, тогда далее сначала удалятся все по IgnoreList, для уменьшения нагрузки генерации 
// 	TArray<FSpawnCell> LGenerationArray = InData;
//
// 	//Если тип обработки вернуть именно количество ПРЕДМЕТОВ, необходимо рассчитать доступное количество
// 	//Доступное количество мы получим путём того что удалим все предметы, которые лежат в IgnoreList
// 	//Если требуемое количество больше доступного, мы вернём что осталось, после исключения IgnoreList-ом, иначе начнется генерация
// 	if (TypeOfQuantity == ETypeOfQuantityProcessing::ETOQP_ByItem)
// 	{
// 		TArray<FSpawnCell> Temp = InData;
// 		for (auto Item : IgnoreList)
// 		{
// 			int32 Idx = 0;
// 			while (Idx != INDEX_NONE)
// 			{
// 				Idx = Temp.Find(Item);
// 				if(Idx == INDEX_NONE) break;
//
// 				Temp.RemoveAt(Idx);
// 			}
// 		}
// 		if (Temp.Num() < CountForGroup)
// 		{
// 			for (auto Data : Temp)
// 			{
// 				RandomItems.Add(Data.ItemSettings);
// 				//MaxCount может быть равен -1, это обозначает игнорировать ограничение на максимальное количество
// 				if (MaxCount > 0 && RandomItems.Num() >= MaxCount) return RandomItems;
// 			}
// 			return RandomItems;
// 		}
// 	}
//
// 	int32 Max = 0;
// 	float Possible = 0.f;
// 	RecalculateWeightMax(Max, Possible, LGenerationArray);
// 	int32 CurrentProcess = 0;
// 	while (CountForGroup != CurrentProcess && LGenerationArray.Num() > 0)
// 	{
// 		int32 LRand = FMath::RandRange(0, Max);
// 		float LResult = float(LRand) / 1000.f;
// 		float CurrentRangeMin = 0;
// 		float CurrentRangeMax = 0;
// 		
// 		for (int32 Idx = 0; Idx < LGenerationArray.Num(); Idx++)
// 		{
// 			FSpawnCell ItemCell = LGenerationArray[Idx];
// 			CurrentRangeMax = CurrentRangeMin + ItemCell.Weight;
// 			if (LResult > CurrentRangeMin && LResult < CurrentRangeMax)
// 			{
// 				//Исключение из IgnoreList (Не требуется если целевая задача выдать предметы, так как массив был заранее отчищен по IgnoreList)
// 				if (!ItemCell.ItemSettings.bIsPrefab && TypeOfQuantity == ETypeOfQuantityProcessing::ETOQP_ByPassage && IgnoreList.Contains(ItemCell.ItemSettings.ItemData.ItemClass)) break;
// 				if (ItemCell.ItemSettings.bIsPrefab)
// 				{
// 					RandomItems.Add(ItemCell.ItemSettings);
// 					continue;
// 				}
// 				switch (TypeCheck)
// 				{
// 				case ETypeCheckClass::EGRT_Ignore:
// 				{
// 					RandomItems.Add(ItemCell.ItemSettings);
// 					break;
// 				};
// 				case ETypeCheckClass::EGRT_ExcludeItem:
// 				{
// 					if (RandomItems.Contains(ItemCell.ItemSettings))
// 					{
// 						LGenerationArray.RemoveAt(Idx);
// 						RecalculateWeightMax(Max, Possible, LGenerationArray);
// 						break;
// 					}
// 					LGenerationArray.RemoveAt(Idx);
// 					RandomItems.Add(ItemCell.ItemSettings);
// 						
// 					RecalculateWeightMax(Max, Possible, LGenerationArray);
// 					break;
// 				};
// 				case ETypeCheckClass::EGRT_IncludeAndSumm:
// 				{
// 					int32 LIdx = RandomItems.Find(ItemCell.ItemSettings);
// 					if (LIdx == INDEX_NONE)
// 					{
// 						RandomItems.Add(ItemCell.ItemSettings);
// 					}
// 					else
// 					{
// 						if (UKismetSystemLibrary::IsValidClass(RandomItems[LIdx].ItemData.ItemClass) && Cast<UBaseItem>(RandomItems[LIdx].ItemData.ItemClass->GetDefaultObject())->IsStackable())
// 						{
// 							int32 StackSize = Cast<UBaseItem>(RandomItems[LIdx].ItemData.ItemClass->GetDefaultObject())->GetStackSize();
// 							if (RandomItems[LIdx].ItemData.Amount > 0)
// 							{
// 								RandomItems[LIdx].ItemData.Amount += ItemCell.ItemSettings.ItemData.Amount;
// 							}
// 							else
// 							{
// 								RandomItems[LIdx].ItemData.Range.X += ItemCell.ItemSettings.ItemData.Range.X;
// 								RandomItems[LIdx].ItemData.Range.Y += ItemCell.ItemSettings.ItemData.Range.Y;
// 							}
// 						}
// 						else
// 						{
// 							break;
// 						}
// 					}
// 					break;
// 				};
// 				}
//
// 				break;
// 			}
//
// 			CurrentRangeMin += ItemCell.Weight;
// 		}
// 		//Подсчет проходов. Если требуется количество предметов, тогда количество проходов приравнивается к количеству сгенерированных предметов
// 		CurrentProcess = (TypeOfQuantity == ETypeOfQuantityProcessing::ETOQP_ByPassage ? (CurrentProcess + 1) : RandomItems.Num());
// 		
// 		//MaxCount может быть равен -1, это обозначает игнорировать ограничение на максимальное количество
// 		if (MaxCount > 0 && MaxCount < CurrentProcess) break;
// 	}
// 	return RandomItems;
// }
//
// void ULootWorldSubSystem::RecalculateWeightMax(int32& Max, float& Possible, TArray<FSpawnCell> Array)
// {
// 	Max = 0;
// 	Possible = 0;
// 	for (FSpawnCell ItemCell : Array)
// 	{
// 		Max += (ItemCell.Weight * 1000);
// 		if (Possible < ItemCell.Weight)
// 		{
// 			Possible = ItemCell.Weight;
// 		}
// 	}
// }
// #pragma endregion PSEUDORANDOM
//
//
// #pragma region TOOLS
//
// void ULootWorldSubSystem::RefreshLootTags()
// {
// 	CurrentLootTags.Reset();
// 	for (auto Controller : CurrentOwners)
// 	{
// 		if (UBaseLootManager* LootManager = UInventoryToolsLibrary::GetComponentFromOwner<UBaseLootManager>(Controller, UBaseLootManager::StaticClass()))
// 		{
// 			CurrentLootTags.AppendTags(LootManager->GetLootTags());
// 		}
// 	}
// }
//
// bool ULootWorldSubSystem::IsRowActive(const FGenerateRowData& RowData) const
// {
// 	if (!RowData.DataTable) return false;
//
// 	if (!RowData.DataTable->FindRow<FSpawnCellsData>(RowData.RowName, "")) return false;
//
// 	if (!RowData.LootTag.IsValid()) return true;
//
// 	if (RowData.EnableState == ERowEnableType::ERET_DisableIf && CurrentLootTags.HasTag(RowData.LootTag))
// 	{
// 		return false;
// 	}
//
// 	if (RowData.EnableState == ERowEnableType::ERET_EnableIf && !CurrentLootTags.HasTag(RowData.LootTag))
// 	{
// 		return false;
// 	}
// 	return true;
// }
//
// void ULootWorldSubSystem::UpdateRowWeight(FGenerateRowData& RowData) const
// {
// 	for (auto RowMod : RowData.WeightModificate)
// 	{
// 		if (CurrentLootTags.HasTag(RowMod.WeightTag))
// 		{
// 			RowData.Weight += RowMod.Weight;
// 		}
// 	}
// }
//
// void ULootWorldSubSystem::ModificateItemByRow(FGenerateRowData& RowData, FMakeItemContext& ItemData)
// {
// 	if (ItemData.bIsPrefab) return;
// 	if (ItemData.ItemData.Range.GetMax() < 0)
// 	{
// 		ItemData.ItemData.Range.X = ItemData.ItemData.Amount;
// 		ItemData.ItemData.Range.Y = ItemData.ItemData.Amount;
// 	}
// 	
// 	for (FItemRowModificate& RowMod : RowData.ItemModificator)
// 	{
// 		if (!CurrentLootTags.HasTag(RowMod.WeightTag)) continue;
// 		
// 		switch (RowMod.WeightBonusType)
// 		{
// 		case (EItemAmoutModificate::EIAB_Add) :
// 		{
// 			ItemData.ItemData.Range.X += RowMod.Weight;
// 			ItemData.ItemData.Range.Y += RowMod.Weight;
// 		}
// 		case (EItemAmoutModificate::EIAB_Substruct) :
// 		{
// 			ItemData.ItemData.Range.X -= RowMod.Weight;
// 			ItemData.ItemData.Range.Y -= RowMod.Weight;
// 		}
// 		case (EItemAmoutModificate::EIAB_AddPercent) :
// 		{
// 			ItemData.ItemData.Range.X = ItemData.ItemData.Range.X + (ItemData.ItemData.Range.X * RowMod.Weight) / 100.f;
// 			ItemData.ItemData.Range.Y = ItemData.ItemData.Range.Y + (ItemData.ItemData.Range.Y * RowMod.Weight) / 100.f;
// 		}
// 		case (EItemAmoutModificate::EIAB_SubstructPercent) :
// 		{
// 			ItemData.ItemData.Range.X = ItemData.ItemData.Range.X - (ItemData.ItemData.Range.X * RowMod.Weight) / 100.f;
// 			ItemData.ItemData.Range.Y = ItemData.ItemData.Range.X - (ItemData.ItemData.Range.Y * RowMod.Weight) / 100.f;
// 		}
// 		default: ;
// 		}
// 	}
// }
//
// #pragma endregion TOOLS