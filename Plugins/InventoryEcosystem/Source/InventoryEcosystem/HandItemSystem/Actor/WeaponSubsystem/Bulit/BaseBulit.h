// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "InventoryEcosystem/HandItemSystem/Library/WeaponLibrary.h"
#include "BaseBulit.generated.h"

UCLASS()
class HANDITEMSYSTEM_API ABaseBulit : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* ProjectileCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBalisticFXDataTable* BalisticFXData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 30.f;

	float Distance = 50000.f;
	FVector Start;

public:	
	ABaseBulit();

	void AddIgnoreActor(AActor* InActor) const;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Launch(FVector InVelocity, float Distance, TSubclassOf<UDamageType> DamageType);

	UFUNCTION()
	virtual void Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UDamageType> DamageType;

protected:
	virtual void BeginPlay() override;

};
