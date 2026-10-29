// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryEcosystem/HandItemSystem/Data/LocomotionDefines.h"
#include "GetCharacterData.generated.h"


UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UGetCharacterData : public UInterface
{
    GENERATED_BODY()
};


class HANDITEMSYSTEM_API IGetCharacterData
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FCharacterAnimationData I_GetAnimationData();
};