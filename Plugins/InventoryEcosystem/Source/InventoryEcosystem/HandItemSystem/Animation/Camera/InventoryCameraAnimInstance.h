// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "InventoryEcosystem/HandItemSystem/Data/LocomotionDefines.h"
#include "InventoryCameraAnimInstance.generated.h"

class UInventoryCameraComponent;
class ABaseCharacter;
class IGetCharacterData;
/**
 * 
 */
UCLASS(Meta = (BlueprintSpawnableComponent),
	HideCategories = ("ComponentTick", "Clothing", "Physics", "MasterPoseComponent", "Collision", "AnimationRig",
		"Lighting", "Deformer", "Rendering", "PathTracing", "HLOD", "Navigation", "VirtualTexture", "SkeletalMesh",
		"LeaderPoseComponent", "Optimization", "LOD", "MaterialParameters", "TextureStreaming", "Mobile", "RayTracing"))
class HANDITEMSYSTEM_API UInventoryCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	
public:
	UInventoryCameraAnimInstance() = default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	ECharacterViewType ViewMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	ECharacterMovementType MovementType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	ECharacterStanceType StanceType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bRightShoulder = true;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	UInventoryCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	ABaseCharacter* Character = nullptr;


	IGetCharacterData* Interface = nullptr;
    
public:
    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaTime) override;
};
