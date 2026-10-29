// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseLineInventory.h"
#include "UObject/Object.h"
#include "LootSaveData.generated.h"

USTRUCT()
struct LOOTSYSTEM_API FContainerData
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	FName ContainerName;

	UPROPERTY(SaveGame)
	bool AlwaysLoad;

	UPROPERTY(SaveGame)
	FDateTime TimeRestart;

	UPROPERTY(SaveGame)
	FInventorySave ContainerSave;

	FContainerData() = default;
	FContainerData(FName InContainerName)
	{
		ContainerName = InContainerName;
	}
	
	friend bool operator==(const FContainerData& Left, const FContainerData& Right)
	{
		return (Left.ContainerName == Right.ContainerName);
	}
};

UCLASS()
class LOOTSYSTEM_API ULootSaveData : public UObject
{
	GENERATED_BODY()
	
	
	
	
};
