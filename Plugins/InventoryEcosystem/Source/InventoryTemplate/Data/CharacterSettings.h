// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSettings.generated.h"


UENUM(BlueprintType)
enum class ECharacterMoveState : uint8
{
	ECMS_Walk			UMETA(DisplayName = "Ходит"),
	ECMS_Run			UMETA(DisplayName = "Бежит"),
	ECMS_Sprint			UMETA(DisplayName = "Спринт"),
	ECMS_Crouch			UMETA(DisplayName = "Присяд"),
	ECMS_Prone			UMETA(DisplayName = "Лежа"),
	ECMS_Swim			UMETA(DisplayName = "Плывет"),
};



USTRUCT(BlueprintType)
struct FLocomotionData
{
	GENERATED_BODY()

public:

	FVector GetSpeed(ECharacterMoveState characterMoveState)
	{
		switch (characterMoveState)
		{
			case ECharacterMoveState::ECMS_Walk:
			return WalkSpeed;
			case ECharacterMoveState::ECMS_Run:
			return RunSpeed;
			case ECharacterMoveState::ECMS_Sprint:
			return SprinSpeed;
			case ECharacterMoveState::ECMS_Crouch:
			return CrouchSpeed;
			case ECharacterMoveState::ECMS_Prone:
			return ProneSpeed;
			case ECharacterMoveState::ECMS_Swim:
			return SwimSpeed;

		}
		
		return FVector::ZeroVector;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector WalkSpeed = FVector(200.f,200.f,200.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector RunSpeed = FVector(420.f,380.f,350.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector SprinSpeed = FVector(600.f,600.f,600.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector CrouchSpeed = FVector(150.f,150.f,150.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector ProneSpeed = FVector(100.f,100.f,100.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NotReplicated)
	FVector SwimSpeed = FVector(150.f,150.f,150.f);
};



UCLASS()
class INVENTORYTEMPLATE_API UCharacterSettings : public UDataAsset
{
	GENERATED_BODY()
	
};
