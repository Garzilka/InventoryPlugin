// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryActorControll.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryActorControll : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HANDITEMSYSTEM_API IInventoryActorControll
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	bool I_FirstAction(bool IsUse);

	UFUNCTION(BlueprintNativeEvent)
	bool I_SecondAction(bool IsUse);

	UFUNCTION(BlueprintNativeEvent)
	bool I_ReloadAction(bool IsReload);

	UFUNCTION(BlueprintNativeEvent)
	bool I_TacticalAction();

	UFUNCTION(BlueprintNativeEvent)
	bool I_MeleeAction();

	UFUNCTION(BlueprintNativeEvent)
	bool I_EquipActor();

	UFUNCTION(BlueprintNativeEvent)
	bool I_UnequipActor();

	UFUNCTION(BlueprintNativeEvent)
	bool I_SwitchScope(bool Next);
};
