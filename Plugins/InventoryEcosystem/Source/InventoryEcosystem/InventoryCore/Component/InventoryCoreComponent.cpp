// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryCoreComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"
#include "InventoryEcosystem/InventoryCore/Library/InventoryCoreLibrary.h"

#pragma region ENGINE

UInventoryCoreComponent::UInventoryCoreComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryCoreComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryCoreComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma endregion ENGINE

#pragma region TOOLS

bool UInventoryCoreComponent::IsLocallyControlled()
{
	return (GetOwningPlayerPawn() && GetOwningPlayerPawn()->IsLocallyControlled());
}

bool UInventoryCoreComponent::IsLocallySpectator()
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

APawn* UInventoryCoreComponent::GetOwningPlayerPawn()
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

APlayerController* UInventoryCoreComponent::GetOwningPlayerController()
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

void UInventoryCoreComponent::ChangeObjectOuter(UObject* Object, AActor* InNewOuter)
{
	if (!(Object && InNewOuter && InNewOuter->HasAuthority())) return;
	if (InNewOuter == Object->GetOuter()) return;
	Object->Rename(nullptr, InNewOuter);
}

USkeletalMeshComponent * UInventoryCoreComponent::GetMesh()
{
    if (IsValid(_PRIVATE_Mesh)) return _PRIVATE_Mesh;
    
    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(GetOwner());
    if (!LInterface) return nullptr;

    _PRIVATE_Mesh = LInterface->Execute_I_GetMesh(GetOwner());
    return _PRIVATE_Mesh;
}

UCapsuleComponent * UInventoryCoreComponent::GetCapsuleComponent()
{
    if (IsValid(_PRIVATE_CapsuleComponent)) return _PRIVATE_CapsuleComponent;
    
    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(GetOwner());
    if (!LInterface) return nullptr;

    _PRIVATE_CapsuleComponent = LInterface->Execute_I_GetCapsuleComponent(GetOwner());
    return _PRIVATE_CapsuleComponent;
}

#pragma endregion TOOLS

