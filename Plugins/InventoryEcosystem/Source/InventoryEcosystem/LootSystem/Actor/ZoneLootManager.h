// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneLootManager.generated.h"



UCLASS()
class LOOTSYSTEM_API AZoneLootManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AZoneLootManager();

protected:
	virtual void BeginPlay() override;

};
