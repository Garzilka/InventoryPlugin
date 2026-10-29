// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseWeaponAnimationInstance.generated.h"

class ABaseFireWeapon;

UCLASS()
class HANDITEMSYSTEM_API UBaseWeaponAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadOnly)
	ABaseWeapon* WeaponOwner = nullptr;

	UPROPERTY(BlueprintReadOnly)
	ABaseFireWeapon* FireWeaponOwner = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool WeaponIsReady = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCharged = false;


public:

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponOwner(ABaseWeapon* NewWeaponOwner);
};
