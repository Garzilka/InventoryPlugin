// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponAnimationInstance.h"

#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseFireWeapon.h"


void UBaseWeaponAnimationInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!(IsValid(WeaponOwner) && IsValid(GetOwningActor())))
	{
		SetWeaponOwner(Cast<ABaseWeapon>(GetOwningActor()));		
		return;
	}
	WeaponIsReady = WeaponOwner->IsReadyToUse();

	if (!FireWeaponOwner) return;
	bIsCharged = FireWeaponOwner->IsCharged();
}

void UBaseWeaponAnimationInstance::SetWeaponOwner(ABaseWeapon* NewWeaponOwner)
{
	if (!NewWeaponOwner) return;

	WeaponOwner = NewWeaponOwner;
	FireWeaponOwner = Cast<ABaseFireWeapon>(NewWeaponOwner);
	if (!IsValid(WeaponOwner)) return;

	WeaponIsReady = WeaponOwner->IsReadyToUse();
}