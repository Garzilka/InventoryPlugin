// Fill out your copyright notice in the Description page of Project Settings.


#include "HandItemLibrary.h"

#include "InventoryEcosystem/HandItemSystem/Interface/InventoryActorControll.h"
#include "Kismet/KismetSystemLibrary.h"

class IInventoryActorControll* UHandItemLibrary::HasControllInterface(UObject* TargetObject)
{
	if (TargetObject == nullptr) return nullptr;
	if (UKismetSystemLibrary::DoesImplementInterface(TargetObject, UInventoryActorControll::StaticClass()))
	{
		return Cast<IInventoryActorControll>(TargetObject);
	}
	return nullptr;
}
