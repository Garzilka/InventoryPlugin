// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HandItemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HANDITEMSYSTEM_API UHandItemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static class IInventoryActorControll* HasControllInterface(UObject* TargetObject);
};
