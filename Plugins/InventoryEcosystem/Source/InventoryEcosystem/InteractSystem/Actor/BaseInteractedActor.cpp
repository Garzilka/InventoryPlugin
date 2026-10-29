// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractedActor.h"
#include "InventoryEcosystem/InteractSystem/Components/BaseInteractComponent.h"

// Sets default values
ABaseInteractedActor::ABaseInteractedActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	InteractComponent = CreateDefaultSubobject<UBaseInteractComponent >(TEXT("Interact Component"));
	InteractComponent->SetupAttachment(RootComponent);
}

void ABaseInteractedActor::BeginPlay()
{
	if (HasAuthority())
	{
		// InteractComponent->AddInteractType(EInteractType::EAIT_Release);
	}
	Super::BeginPlay();
}

void ABaseInteractedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

