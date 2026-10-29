// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "Engine/ActorChannel.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

#include "InventoryCoreSkeletalMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYCORE_API UInventoryCoreSkeletalMeshComponent : public USkeletalMeshComponent
{
    GENERATED_BODY()

public:
    UInventoryCoreSkeletalMeshComponent() = default;
    
    UFUNCTION(BlueprintPure)
    bool HasAuthority() const { return (GetOwner() && GetOwner()->HasAuthority()); };
    
    UFUNCTION(BlueprintPure)
    bool IsLocallyControlled();
    
    UFUNCTION(BlueprintPure)
    bool IsLocallySpectator();
    
    UFUNCTION(BlueprintPure)
    APawn* GetOwningPlayerPawn();
    
    UFUNCTION(BlueprintPure)
    APlayerController* GetOwningPlayerController();

    UFUNCTION(BlueprintPure)
    USkeletalMeshComponent* GetPawnMesh();

    UFUNCTION(BlueprintPure)
    UCapsuleComponent* GetCapsuleComponent();
    
private:
    UPROPERTY()
    APawn* PawnOwner = nullptr;

    UPROPERTY()
    APlayerController* ControllerOwner = nullptr;

    UPROPERTY()
    USkeletalMeshComponent* _PRIVATE_Mesh = nullptr;

    UPROPERTY()
    UCapsuleComponent* _PRIVATE_CapsuleComponent = nullptr;

};
