// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Components/ActorComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/NetDriver.h"
#include "Engine/ActorChannel.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

#include "InventoryCoreComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYCORE_API UInventoryCoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UInventoryCoreComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
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
    USkeletalMeshComponent* GetMesh();

    UFUNCTION(BlueprintPure)
    UCapsuleComponent* GetCapsuleComponent();
    
    void ChangeObjectOuter(UObject* ItemRef, AActor* InNewOuter);
protected:
	virtual void BeginPlay() override;

	bool CanSerialize()
	{
		if ((!GEngine || !GetOwner() || HasAnyFlags(RF_ArchetypeObject)))
			return false;
		if ((GetWorld() && GetWorld()->WorldType == EWorldType::EditorPreview))
			return false;
		if (!HasAuthority())
			return false;
		return true;
	};
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
