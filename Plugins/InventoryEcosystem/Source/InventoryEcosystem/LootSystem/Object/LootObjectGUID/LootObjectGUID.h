// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "UObject/Object.h"
#include "LootObjectGUID.generated.h"

/**
 * 
 */
UCLASS()
class LOOTSYSTEM_API ULootObjectGUID : public UCoreInventoryObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere)
    FName GUID = NAME_None;
    
    UPROPERTY(Transient)
    FLootSettingsData LootSettingsData = FLootSettingsData();
    
    UPROPERTY(Transient)
    FGenerateSettings GenerateSettings = FGenerateSettings();
public:
    ULootObjectGUID();

    const FLootSettingsData& GetLootSettingsData();
    const FGenerateSettings& GetGenerateSettings();
    bool IsUniqueLoot();
    EGenerationLootType GetRespawnLootType();
    float GetTimeToRestart();

    UFUNCTION(CallInEditor)
    void AddToLootSystem();
};
