// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"

#include "InventoryEcosystem/LootSystem/Data/LootSaveData.h"
#include "BaseLootManager.generated.h"

class ABaseContainer;
class UBaseLootComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOOTSYSTEM_API UBaseLootManager : public UInventoryCoreComponent
{
	GENERATED_BODY()

protected:	
	UPROPERTY()
	FGameplayTagContainer LootTags;

	UPROPERTY(SaveGame)
	TArray<FContainerData> ContainerSaveData;
public:	
	UBaseLootManager();

	UFUNCTION(BlueprintCallable)
	bool SetLootTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	bool RemoveLootTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	bool IsActiveTag(FGameplayTag Tag) const;

	const FGameplayTagContainer& GetLootTags() const;

	void AddBy(UBaseLootComponent* Container, FInventorySave ContainerRespawnData);
	
	FContainerData GetDataBy(UBaseLootComponent* Container); 
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetLootModifyLVL(FGameplayTag Tag);
	void Server_SetLootModifyLVL_Implementation(FGameplayTag Tag);
};
