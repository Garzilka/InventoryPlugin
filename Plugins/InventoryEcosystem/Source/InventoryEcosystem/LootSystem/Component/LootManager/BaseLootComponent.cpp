// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLootComponent.h"

#include "BaseLootManager.h"
#include "Engine/World.h"
#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseLineInventory.h"
#include "InventoryEcosystem/LootSystem/WorldSubSystem/LootWorldSubSystem.h"

UBaseLootComponent::UBaseLootComponent(){}

void UBaseLootComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

#if WITH_EDITOR	
	if (!IsInGameThread() &&  !HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) && !HasAnyFlags(RF_ArchetypeObject) && !HasAnyFlags(RF_MirroredGarbage))
	{
	    if (GetWorld() && GetWorld()->IsGameWorld()) return;
	    
	    if (LootObjectGUID == nullptr)
	    {
	        LootObjectGUID = NewObject<ULootObjectGUID>(this);
	        LootObjectGUID->AddToLootSystem();
	        Modify();
	        GetOwner()->Modify();
	    }
	}
#endif
}

bool UBaseLootComponent::IsUniqueLoot()
{
    if (!LootObjectGUID) return false;
    
    return LootObjectGUID->IsUniqueLoot();
}

EGenerationLootType UBaseLootComponent::GetRespawnLootType()
{
    if (!LootObjectGUID) return EGenerationLootType::EGLT_None;
    
    return LootObjectGUID->GetRespawnLootType();
}

float UBaseLootComponent::GetTimeToRestart()
{
    if (!LootObjectGUID) return -1.f;
    
    return LootObjectGUID->GetTimeToRestart(); 
}

void UBaseLootComponent::InitToLoodData()
{
    if (LootObjectGUID)
    {
    	LootObjectGUID->AddToLootSystem();
    }
}

TArray<UBaseItem*> UBaseLootComponent::GenereteItems(APlayerController* For, bool WithLoad)
{
    if (GetRespawnLootType() == EGenerationLootType::EGLT_None) return {};

    TArray<UBaseItem*> Result;
    
    if (WithLoad && GetRespawnLootType() != EGenerationLootType::EGLT_BeginPlay)
    {
        if (LoadData(For, Result))
        {
            return Result;
        }
    }

    
    if (ULootWorldSubSystem* SubSystem = GetWorld()->GetSubsystem<ULootWorldSubSystem>())
    {
        Result = SubSystem->GenerateItems(LootObjectGUID->GetGenerateSettings(), (For == nullptr ? GetOwner() : For));
    }
    return Result;
}

bool UBaseLootComponent::LoadData(APlayerController* For, TArray<UBaseItem*>& Result)
{
    if (!For) return false;
    // if (!CurrentGenerateData.bIsUniqueLoot)
    // {
    // 	FTimespan Result = (Date_RespawnLoot - FDateTime::UtcNow());
    // 	return Result.GetTotalSeconds() > 0;
    // }
    Result.Empty();

    if (UBaseLootManager* LootManager = UInventoryToolsLibrary::GetComponentFromOwner<UBaseLootManager>(For, UBaseLootManager::StaticClass()))
    {
        FContainerData Data = LootManager->GetDataBy(this);
        FTimespan TimeResult = (Data.TimeRestart - FDateTime::UtcNow());
        
        if (Data.ContainerName != FName() && (TimeResult.GetTotalSeconds() > 0 || Data.AlwaysLoad))
        {
            for (int32 Idx = 0; Idx < Data.ContainerSave.SavedData.Num(); Idx++)
            {
                for (auto Item : UInventoryLibrary::MakeItems(GetOwner(), FInventoryItemData(Data.ContainerSave.SavedData[Idx].SavedClass)))
                {
                    if (!IsValid(Item)) continue;
                    Result.Add(Item);
                    
                    FMemoryReader MemoryReader(Data.ContainerSave.SavedData[Idx].ItemData, true);
                    FSaveGameArchive ArLoad(MemoryReader);
                    Item->BeginPlay();
                    Item->Serialize(ArLoad);
                }
            }
            return true;
        }
    }
    return false;
}

void UBaseLootComponent::SaveDataFor(AController* PlayerController, FInventorySave SaveData)
{
    if (!(IsValid(PlayerController) && IsValid(PlayerController->GetOwner()))) return;

    if (UBaseLootManager* LootManager = UInventoryToolsLibrary::GetComponentFromOwner<UBaseLootManager>(PlayerController, UBaseLootManager::StaticClass()))
    {
        LootManager->AddBy(this, SaveData);
    }
}

void UBaseLootComponent::BeginPlay()
{
	Super::BeginPlay();
}
