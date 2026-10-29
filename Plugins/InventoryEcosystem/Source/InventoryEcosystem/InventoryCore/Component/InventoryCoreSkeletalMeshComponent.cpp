// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryCoreSkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"
#include "InventoryEcosystem/InventoryCore/Library/InventoryCoreLibrary.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


#pragma region TOOLS

bool UInventoryCoreSkeletalMeshComponent::IsLocallyControlled()
{
	return (GetOwningPlayerPawn() && GetOwningPlayerPawn()->IsLocallyControlled());
}

bool UInventoryCoreSkeletalMeshComponent::IsLocallySpectator()
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

APawn* UInventoryCoreSkeletalMeshComponent::GetOwningPlayerPawn()
{
    if (IsValid(PawnOwner)) return PawnOwner;
    
    PawnOwner = Cast<APawn>(GetOwner());
    
    if (!PawnOwner)
    {
        if (!ControllerOwner)
        {
            ControllerOwner = Cast<APlayerController>(GetOwner());
        }
        if (ControllerOwner)
        {
    	    PawnOwner = Cast<APawn>(ControllerOwner->GetPawn());            
        }
    }
    
    return PawnOwner;
}

APlayerController* UInventoryCoreSkeletalMeshComponent::GetOwningPlayerController()
{
    if (IsValid(ControllerOwner)) return ControllerOwner;
    
    ControllerOwner = Cast<APlayerController>(GetOwner());
    
    if (!ControllerOwner)
    {
        if (!PawnOwner)
        {
            PawnOwner = Cast<APawn>(GetOwner());
        }
        if (PawnOwner)
        {
            ControllerOwner = Cast<APlayerController>(PawnOwner->GetController());       
        }
    }
    
    return ControllerOwner;
}

USkeletalMeshComponent* UInventoryCoreSkeletalMeshComponent::GetPawnMesh()
{
    if (IsValid(_PRIVATE_Mesh)) return _PRIVATE_Mesh;
    
    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(GetOwningPlayerPawn());
    if (!LInterface) return nullptr;

    _PRIVATE_Mesh = LInterface->Execute_I_GetMesh(GetOwningPlayerPawn());
    return _PRIVATE_Mesh;
}

UCapsuleComponent * UInventoryCoreSkeletalMeshComponent::GetCapsuleComponent()
{
    if (IsValid(_PRIVATE_CapsuleComponent)) return _PRIVATE_CapsuleComponent;
    
    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(GetOwningPlayerPawn());
    if (!LInterface) return nullptr;

    _PRIVATE_CapsuleComponent = LInterface->Execute_I_GetCapsuleComponent(GetOwningPlayerPawn());
    return _PRIVATE_CapsuleComponent;
}

#pragma endregion TOOLS

