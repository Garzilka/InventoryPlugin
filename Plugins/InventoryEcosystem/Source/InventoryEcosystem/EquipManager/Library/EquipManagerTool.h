// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "EquipManagerTool.generated.h"

/**
 * 
 */
UCLASS()
class EQUIPMANAGER_API UEquipManagerTool : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FGameplayTagContainer GetClothTags();
	static FGameplayTagContainer GetBodyTags();
};
