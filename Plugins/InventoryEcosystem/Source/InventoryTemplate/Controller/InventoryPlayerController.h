// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootManager.h"
#include "InventoryEcosystem/InventoryManagerSystem/Component/BaseInventoryManager.h"
#include "InventoryPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYTEMPLATE_API AInventoryPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseComponent, meta = (AllowPrivateAccess = "true"))
	UBaseInventoryManager* InventoryManager;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseComponent, meta = (AllowPrivateAccess = "true"))
	UBaseLootManager* LootManager;

public:
	AInventoryPlayerController();
	UBaseInventoryManager* GetInventoryManager() { return InventoryManager; }
	UBaseLootManager* GetLootManager() { return LootManager; }
};
