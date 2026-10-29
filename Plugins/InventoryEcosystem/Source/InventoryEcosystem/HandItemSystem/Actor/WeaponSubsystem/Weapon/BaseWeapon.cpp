// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	RotateWepaon = CreateDefaultSubobject<USceneComponent>(TEXT("RotateWepaon"));
	RotateWepaon->SetupAttachment(GetRootComponent());
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		OnRep_WeaponData();
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, Mods);
	DOREPLIFETIME(ABaseWeapon, WeaponData);
}

#pragma endregion ENGINE

#pragma region WEAPON_INIT

void ABaseWeapon::InitWeapon(UBaseWeaponData* InWeaponData)
{
	if (!InWeaponData) return;

	WeaponData = InWeaponData;
	OnRep_WeaponData();
}

#pragma endregion

#pragma region TOOLS

ACharacter* ABaseWeapon::GetCharacterOwner()
{
	if (CharacterOwner == nullptr) CharacterOwner = Cast<ACharacter>(GetOwner());
	return CharacterOwner;
}

bool ABaseWeapon::IsLocallyControlled()
{
	if (!GetCharacterOwner()) return false;
	return GetCharacterOwner()->IsLocallyControlled();
}

bool ABaseWeapon::I_FirstAction_Implementation(bool IsUse)
{
	if (!Use(IsUse)) return false;

	if (!HasAuthority())
	{
		Server_Use(IsUse);
	}
	
	return true;
}

TArray<AActor*> ABaseWeapon::GetModules()
{
	TArray<AActor*> LMods;
	for (auto ModData : Mods)
	{
		if (!IsValid(ModData.ActorModuleInfo)) continue;
		LMods.Add(ModData.ActorModuleInfo);
	}
	return LMods;
}

USkeletalMeshComponent* ABaseWeapon::GetWeaponMesh()
{
	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(MeshComponent))
	{
		MeshComponent->SetVisibility(false);
		MeshComponent->DestroyComponent();
		MeshComponent->UnregisterComponent();
		MeshComponent->MarkAsGarbage();
		MeshComponent = nullptr;
	}
	if (MeshComponent == nullptr)
	{
		MeshComponent = NewObject<USkeletalMeshComponent>(this);
		MeshComponent->RegisterComponentWithWorld(GetWorld());
		MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		MeshComponent->SetCollisionProfileName(UInventoryLibrary::GetDefaultInteractProfile());
		MeshComponent->ComponentTags.Add("OutLine");
	}
	return Cast<USkeletalMeshComponent>(MeshComponent);
}

void ABaseWeapon::OnRep_WeaponData(){}

void ABaseWeapon::AddCaptureRotate(FVector2D AddRot)
{
	RotateWepaon->SetWorldRotation(UKismetMathLibrary::ComposeRotators(RotateWepaon->GetComponentRotation(), FRotator(0.f, (AddRot.X), AddRot.Y)));
}

void ABaseWeapon::ResetCaptureRotate()
{
	RotateWepaon->SetRelativeRotation(FRotator::ZeroRotator);
	GetWeaponMesh()->SetRelativeLocation(WeaponData->RenderOffsetLocationMesh);
}

void ABaseWeapon::ActivateRenderTarget()
{
	if (!WeaponData) return;
	GetWeaponMesh()->AttachToComponent(RotateWepaon, FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetWeaponMesh()->SetRelativeLocation(WeaponData->RenderOffsetLocationMesh);
}

void ABaseWeapon::DeactivateRenderTarget()
{
	MeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetWeaponMesh()->SetRelativeLocationAndRotation(WeaponData->LocationMeshOffset, WeaponData->RotationMeshOffset);
}
#pragma endregion TOOLS
