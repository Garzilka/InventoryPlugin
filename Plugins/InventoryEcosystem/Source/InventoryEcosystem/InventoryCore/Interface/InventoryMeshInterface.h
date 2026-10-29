// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "UObject/Interface.h"
#include "InventoryMeshInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInventoryMeshInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class INVENTORYCORE_API IInventoryMeshInterface
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    UFUNCTION(BlueprintNativeEvent)
    USkeletalMeshComponent* I_GetMesh();

    UFUNCTION(BlueprintNativeEvent)
    UCapsuleComponent* I_GetCapsuleComponent();
};