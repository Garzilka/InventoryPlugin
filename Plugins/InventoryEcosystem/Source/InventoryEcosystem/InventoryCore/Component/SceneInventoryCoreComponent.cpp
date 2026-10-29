// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneInventoryCoreComponent.h"

USceneInventoryCoreComponent::USceneInventoryCoreComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void USceneInventoryCoreComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USceneInventoryCoreComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma region TOOLS

bool USceneInventoryCoreComponent::IsLocallyControlled()
{
	return (GetOwningPlayerPawn() && GetOwningPlayerPawn()->IsLocallyControlled());
}

bool USceneInventoryCoreComponent::IsLocallySpectator()
{
	if (!IsValid(GetWorld())) return false;
	APlayerController* LPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(LPC)) return false;

	if (LPC->IsInState(NAME_Spectating))
	{
		return (LPC->GetViewTarget() == GetOwningPlayerPawn());
	}
	return false;
}

APawn* USceneInventoryCoreComponent::GetOwningPlayerPawn()
{
    if (IsValid(PawnOwner)) return PawnOwner;
    
    PawnOwner = Cast<ACharacter>(GetOwner());
    
    if (!PawnOwner)
    {
        if (!ControllerOwner)
        {
            ControllerOwner = Cast<APlayerController>(GetOwner());
        }
        if (ControllerOwner)
        {
            PawnOwner = Cast<ACharacter>(ControllerOwner->GetPawn());            
        }
    }
    
    return PawnOwner;
}

APlayerController* USceneInventoryCoreComponent::GetOwningPlayerController()
{
    if (IsValid(ControllerOwner)) return ControllerOwner;
    
    ControllerOwner = Cast<APlayerController>(GetOwner());
    
    if (!ControllerOwner)
    {
        if (!PawnOwner)
        {
            PawnOwner = Cast<ACharacter>(GetOwner());
        }
        if (PawnOwner)
        {
            ControllerOwner = Cast<APlayerController>(PawnOwner->GetController());       
        }
    }
    
    return ControllerOwner;
}

#pragma endregion TOOLS