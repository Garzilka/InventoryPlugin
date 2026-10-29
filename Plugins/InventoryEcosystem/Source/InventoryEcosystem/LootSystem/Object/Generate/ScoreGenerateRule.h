// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryCore/Object/CoreInventoryObject.h"
#include "ScoreGenerateRule.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class LOOTSYSTEM_API UScoreGenerateRule : public UCoreInventoryObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generate rules", meta = (DisplayPriority = -1, DisplayName = "Использовать бонусы генерации"))
	bool bIsUseBoostItem = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Generate rules")
	int32 GetPlayerScore(APlayerController* For);
	int32 GetPlayerScore_Implementation(APlayerController* For)
	{   
		ensure(false);
		return -1;
	};

	UFUNCTION(BlueprintImplementableEvent, Category = "Generate rules")
	TArray<TSubclassOf<UBaseItem>> GetIngoreListClass(APlayerController* For);
	TArray<TSubclassOf<UBaseItem>> GetIngoreListClass_Implementation(APlayerController* For) //Игнорировать изученные рецепты и карточки "Карнаж"
	{
		return TArray<TSubclassOf<UBaseItem>>();
	};
};
