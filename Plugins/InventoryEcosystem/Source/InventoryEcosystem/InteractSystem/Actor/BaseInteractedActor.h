// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractedActor.generated.h"

UCLASS(BlueprintType, HideCategories = ("Actor Tick", "Rendering", "Replication", "Input", "Actor", "Base", "Collision", "Shape", "MeshComponent", "Cooking", "LOD", "WorldPartition"))
class INTERACTSYSTEM_API ABaseInteractedActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseComponent)
	class UBaseInteractComponent* InteractComponent;

public:	
	ABaseInteractedActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

};
