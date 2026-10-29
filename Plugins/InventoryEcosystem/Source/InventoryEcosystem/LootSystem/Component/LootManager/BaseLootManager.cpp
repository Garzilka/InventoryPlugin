// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLootManager.h"

#include "InventoryEcosystem/LootSystem/Actor/BaseContainer.h"
#include "Engine/World.h"


UBaseLootManager::UBaseLootManager(){}

void UBaseLootManager::BeginPlay()
{
	Super::BeginPlay();
}

bool UBaseLootManager::SetLootTag(FGameplayTag Tag)
{
	if (!HasAuthority())
	{
		return false;
	}
	LootTags.AddTag(Tag);
	return true;
}

void UBaseLootManager::Server_SetLootModifyLVL_Implementation(FGameplayTag Tag)
{
	SetLootTag(Tag);
}

bool UBaseLootManager::RemoveLootTag(FGameplayTag Tag)
{
	if (!HasAuthority()) return false;
	LootTags.RemoveTag(Tag);
	return true;
}

bool UBaseLootManager::IsActiveTag(FGameplayTag Tag) const
{
	return LootTags.HasTag(Tag);
}

const FGameplayTagContainer& UBaseLootManager::GetLootTags() const
{
	return LootTags;
}

void UBaseLootManager::AddBy(UBaseLootComponent* Component, FInventorySave ContainerRespawnData)
{
    if (!IsValid(Component)) return;
    const FName NameContainer = FName(GetWorld()->GetName() + Component->GetOwner()->GetName());
    auto Data = ContainerRespawnData;
	
    int32 Idx = ContainerSaveData.Find(FContainerData(NameContainer));
    if (Idx == INDEX_NONE)
    {
        FContainerData LData;
        LData.ContainerName = NameContainer;
        LData.TimeRestart = (FDateTime::UtcNow() + FTimespan(0, 0, Component->GetTimeToRestart()));
        LData.AlwaysLoad = (Component->GetRespawnLootType() == EGenerationLootType::EGLT_OnlyOne);
        LData.ContainerSave = ContainerRespawnData;
        ContainerSaveData.Add(LData);
        return;
    }
	
    ContainerSaveData[Idx].TimeRestart = (FDateTime::UtcNow() + FTimespan(0, 0, Component->GetTimeToRestart()));
    ContainerSaveData[Idx].ContainerSave = ContainerRespawnData;
}

FContainerData UBaseLootManager::GetDataBy(UBaseLootComponent* LootComponent)
{
    if (!IsValid(LootComponent)) return FContainerData();
    
    const FName NameContainer = FName(GetWorld()->GetName() + LootComponent->GetOwner()->GetName());
    int32 Idx = ContainerSaveData.Find(FContainerData(NameContainer));
    return (Idx == INDEX_NONE ? FContainerData() : ContainerSaveData[Idx]);
}
