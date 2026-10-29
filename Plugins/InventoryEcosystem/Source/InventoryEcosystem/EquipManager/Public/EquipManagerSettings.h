// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "EquipManagerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Equip System"))
class EQUIPMANAGER_API UEquipManagerSettings : public UDeveloperSettings
{

	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Equip system settings")
	FGameplayTag ContainerBody;
};
