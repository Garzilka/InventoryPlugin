// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InventoryEcosystem/InteractSystem/Components/BaseInteractManager.h"
#include "InventoryEcosystem/HandItemSystem/Character/BaseCharacter.h"
#include "InventoryEcosystem/EquipManager/Component/BaseEquipManager.h"
#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootManager.h"
#include "InventoryEcosystem/LootSystem/Component/Trade/BaseTradeManager.h"
#include "InventoryEcosystem/InventoryManagerSystem/Component/BaseInventoryManager.h"
#include "BaseInventoryCharacter.generated.h"

struct FInputActionValue;




UCLASS(BlueprintType, HideCategories = ("Actor Tick", "Rendering", "Replication", "Input", "Actor", "Base", "Collision", "Shape", "MeshComponent", "Cooking", "LOD", "WorldPartition"))
class INVENTORYTEMPLATE_API ABaseInventoryCharacter : public ABaseCharacter
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseComponent, meta = (AllowPrivateAccess = "true"))
	UBaseEquipManager* EquipManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseComponent, meta = (AllowPrivateAccess = "true"))
	UBaseInteractManager* BaseInteractManager;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Action_Interact;

public:
	ABaseInventoryCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UBaseEquipManager* GetEquipManager() {return EquipManager;}
protected:
	virtual void BeginPlay() override;
	virtual bool CanBeMove() override;
	virtual void Input_Interact(const FInputActionValue& ActionValue);
};
