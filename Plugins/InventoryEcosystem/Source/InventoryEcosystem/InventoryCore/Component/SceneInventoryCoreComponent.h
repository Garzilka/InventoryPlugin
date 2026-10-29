// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/NetDriver.h"
#include "Engine/ActorChannel.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

#include "SceneInventoryCoreComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYCORE_API USceneInventoryCoreComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	USceneInventoryCoreComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	bool HasAuthority() { return (GetOwner() && GetOwner()->HasAuthority()); };

	UFUNCTION(BlueprintPure)
	bool IsLocallyControlled();

	UFUNCTION(BlueprintPure)
	bool IsLocallySpectator();

	UFUNCTION(BlueprintCallable)
	APawn* GetOwningPlayerPawn();

	UFUNCTION(BlueprintCallable)
	APlayerController* GetOwningPlayerController();
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APawn* PawnOwner = nullptr;

	UPROPERTY()
	APlayerController* ControllerOwner = nullptr;
};
