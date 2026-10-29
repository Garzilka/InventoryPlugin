// Fill out your copyright notice in the Description page of Project Settings.


#include "LootObjectGUID.h"

#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"

ULootObjectGUID::ULootObjectGUID()
{}

const FLootSettingsData& ULootObjectGUID::GetLootSettingsData()
{
    if (LootSettingsData.MetaData.Object_Name.ToString() == "" && ULootSystemSetting::Get())
    {
        auto LootData = ULootSystemSetting::Get()->LootData.LoadSynchronous();
        auto LLootDataSettings = LootData->GetLootSettingsData().Find(GUID);
        if (!LLootDataSettings) return LootSettingsData;

        LootSettingsData = *LLootDataSettings;
    }
    return LootSettingsData;
}

const FGenerateSettings & ULootObjectGUID::GetGenerateSettings()
{
    if (GenerateSettings.GenerateBlocks.Num() == 0 && ULootSystemSetting::Get())
    {
        auto LootData = ULootSystemSetting::Get()->LootData;
        if (GetLootSettingsData().bUsePreset)
        {
            auto LLootPresets = LootData->LootPresets.Find(GetLootSettingsData().PresetGUID);
            if (!LLootPresets) return GenerateSettings;
            GenerateSettings = LLootPresets->GenerateSettings;
        }
        else
        {
            GenerateSettings = GetLootSettingsData().GenerateSettings;    
        }
    }
    return GenerateSettings;
}

bool ULootObjectGUID::IsUniqueLoot()
{
    return GetGenerateSettings().bIsUniqueLoot;
}

EGenerationLootType ULootObjectGUID::GetRespawnLootType()
{
    return GetGenerateSettings().RespawnSettings.RespawnLootType;
}

float ULootObjectGUID::GetTimeToRestart()
{
    return GetGenerateSettings().RespawnSettings.TimeToRestart; 
}

void ULootObjectGUID::AddToLootSystem()
{
    const ULootSystemSetting* Settings = ULootSystemSetting::Get();
    if (!Settings) return;

    auto LootData = Settings->LootData.LoadSynchronous();
    if (!LootData) return;
	
    if (LootData->LootSettings.Contains(GUID))
    {
        return;
    }
	
    GUID = FName(FGuid::NewGuid().ToString());
    FName Name = NAME_None;

    UActorComponent* LComponent = Cast<UActorComponent>(GetOuter());
    UDataAsset* LDataAsset = Cast<UDataAsset>(GetOuter());
    if (LComponent && LComponent->GetOuter())
    {
        Name = LComponent->GetOwner()->GetFName();
    }
    if (LDataAsset)
    {
        Name = LDataAsset->GetFName();
    }
    if (!LDataAsset && !LComponent)
    {
        Name = GUID;
    }
	
    while (!LootData->AddLootObject(GUID, Name))
    {
        GUID = FName(FGuid::NewGuid().ToString());
    }
    if (LComponent && LComponent->GetOuter())
    {
        LComponent->GetOwner()->Modify(true);
        LComponent->Modify(true);
    }
    if (LDataAsset)
    {
        LDataAsset->Modify(true);
    }
    Modify(true);
}
