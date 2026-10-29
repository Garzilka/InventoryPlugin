// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInventoryCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryMathLib.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager//BaseHandItemManager.h"


ABaseInventoryCharacter::ABaseInventoryCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	EquipManager = CreateDefaultSubobject<UBaseEquipManager>(TEXT("Equip Manager"));
	BaseInteractManager = CreateDefaultSubobject<UBaseInteractManager>(TEXT("Interact Manager"));
}

void ABaseInventoryCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ABaseInventoryCharacter::CanBeMove()
{
	return (Super::CanBeMove() && BaseInteractManager && BaseInteractManager->CharacterInActiveAction());
}

void ABaseInventoryCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ViewState.Rotation = GetActorRotation();
}

void ABaseInventoryCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return;
	
	Subsystem->ClearAllMappings();  
	Subsystem->AddMappingContext(InputMapping, 0);
	auto* EnhancedInput{ Cast<UEnhancedInputComponent>(PlayerInputComponent) };
	
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(Action_Interact, ETriggerEvent::Triggered, this, &ThisClass::Input_Interact);
	}
}

void ABaseInventoryCharacter::Input_Interact(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<bool>()};
	if (Value)
	{
		BaseInteractManager->InteractPressed();
	}
	else
	{
		BaseInteractManager->InteractReleased();
	}
}
