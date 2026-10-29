// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponLibrary.generated.h"


UCLASS()
class UWeaponLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void BalisticFX(AActor* Instigator, FHitResult IntHit, class UBalisticFXDataTable* BalisticFXData);
};
