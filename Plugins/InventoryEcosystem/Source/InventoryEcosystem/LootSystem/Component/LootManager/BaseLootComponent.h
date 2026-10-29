// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/LootSystem/Object/LootObjectGUID/LootObjectGUID.h"
#include "Components/ActorComponent.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseLineInventory.h"
#include "BaseLootComponent.generated.h"


// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateContainerSignature, class ABaseContainer*, User);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LOOTSYSTEM_API UBaseLootComponent : public UActorComponent
{
	GENERATED_BODY()

protected:	
    UPROPERTY(VisibleAnywhere, NonPIEDuplicateTransient)
    ULootObjectGUID* LootObjectGUID = nullptr;
	
public:
    UBaseLootComponent();
    virtual void Serialize(FArchive& Ar) override;

    bool IsUniqueLoot();
    EGenerationLootType GetRespawnLootType();
    float GetTimeToRestart();
    TArray<UBaseItem*> GenereteItems(APlayerController* For, bool WithLoad = true);	
    bool LoadData(APlayerController* For, TArray<UBaseItem*>& Result);
    
    UFUNCTION(CallInEditor)
    void InitToLoodData();

    void SaveDataFor(AController* PlayerController, FInventorySave SaveData);
protected:
	virtual void BeginPlay() override;
};
