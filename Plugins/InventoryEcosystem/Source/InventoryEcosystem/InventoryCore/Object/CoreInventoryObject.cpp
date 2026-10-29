// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreInventoryObject.h"

#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/InventoryCore/Library/InventoryCoreLibrary.h"

#pragma region ENGINE

void UCoreInventoryObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCoreInventoryObject, RepOuter);
	DOREPLIFETIME(UCoreInventoryObject, ParrentObject);
}

bool UCoreInventoryObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	if (!GetOwner()) return false;
	UNetDriver* NetDriver = GetOwner()->GetNetDriver();
	if (!NetDriver) return false;
	NetDriver->ProcessRemoteFunction(GetOwner(), Function, Parms, OutParms, Stack, this);
	return true;
}

int32 UCoreInventoryObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	return (GetOuter() ? GetOuter()->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

#pragma endregion

#pragma region RENAME 

bool UCoreInventoryObject::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
{
	CharacterOwner = nullptr;
	ControllerOwner = nullptr;

	if (HasAuthority())
	{
		RepOuter = NewOuter;
	}
	return Super::Rename(NewName, NewOuter, Flags);
}

void UCoreInventoryObject::OnRep_Outer()
{
	if (RepOuter == nullptr) return;

	if (!HasAuthority())
	{
		Rename(nullptr, RepOuter);
	}
}

#pragma endregion RENAME

#pragma region TOOLS

AActor* UCoreInventoryObject::GetOwner()
{
	if (Owner != GetOuter())
	{
		Owner = Cast<AActor>(GetOuter());
	}
	return Owner;
}

APlayerController* UCoreInventoryObject::GetOwningPlayer()
{
	if (IsValid(ControllerOwner)) return ControllerOwner;

	ControllerOwner = Cast<APlayerController>(GetOwner());

	if (!ControllerOwner && GetOwningPlayerPawn())
	{
		ControllerOwner = Cast<APlayerController>(GetOwningPlayerPawn()->GetController());
	}

	return ControllerOwner;
}

ACharacter* UCoreInventoryObject::GetOwningPlayerPawn()
{
	if (IsValid(CharacterOwner)) return CharacterOwner;

	CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner && GetOwningPlayer())
	{
		CharacterOwner = Cast<ACharacter>(GetOwningPlayer()->GetPawn());
	}
	return CharacterOwner;
}

bool UCoreInventoryObject::SwitchParrent(UObject* NewParrent)
{
	if (IsParrentEqual(NewParrent)) return true;
	if (ParrentObject)
	{
		IItemManagmentInterface* ManageInterface = UInventoryCoreLibrary::HasManagmentInterface(ParrentObject);
		if (!ensure(ManageInterface))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Error, "Error: The inventory system cannot remove the item from the owner. The owner does not have a product management interface\n Transfer refused");
			return false;
		}
		if (!ManageInterface->Execute_I_RemoveItem(ParrentObject, this))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Warning, "Warning: The inventory system cannot remove the item from the owner. The RemoveItem() function returns false. Transfer refused");
			return false;
		}
	}
	else
	{
		LOG_BIO(E_IES_LOG_TYPE::EIEST_Warning, "Warning: ItemOwner == nullptr");
	}
	ParrentObject = NewParrent;

	if (ParrentObject)
	{
		IItemManagmentInterface* ManageInterface = UInventoryCoreLibrary::HasManagmentInterface(ParrentObject);
		if (!ensure(ManageInterface))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Error, "Error: The new owner does not have a product management interface");
			return true;
		}
	}

	return true;
}

bool UCoreInventoryObject::RemoveFromParrent()
{
	if (ParrentObject)
	{
		IItemManagmentInterface* ManageInterface = UInventoryCoreLibrary::HasManagmentInterface(ParrentObject);
		if (!ensure(ManageInterface))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Error, "Error: The inventory system cannot remove the item from the owner. The owner does not have a product management interface\n Transfer refused");
			return false;
		}
		if (!ManageInterface->Execute_I_RemoveItem(ParrentObject, this))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Warning, "Warning: The inventory system cannot remove the item from the owner. The RemoveItem() function returns false. Transfer refused");
			return false;
		}
	}
	else
	{
		LOG_BIO(E_IES_LOG_TYPE::EIEST_Warning, "Warning: ItemOwner == nullptr");
	}
	return true;
}

bool UCoreInventoryObject::IsCanRemoveFromParrent()
{
	if (!ParrentObject) return true;
	if (ParrentObject)
	{
		IItemManagmentInterface* ManageInterface = UInventoryCoreLibrary::HasManagmentInterface(ParrentObject);
		if (!ensure(ManageInterface))
		{
			LOG_BIO(E_IES_LOG_TYPE::EIEST_Error, "Error: The inventory system cannot remove the item from the owner. The owner does not have a product management interface\n Transfer refused");
			return false;
		}
		return ManageInterface->Execute_I_CanRemoveItem(ParrentObject, this);
	}
	return false;
}

void UCoreInventoryObject::ChangeOwner(AActor* InNewOuter)
{
	if (!(InNewOuter && InNewOuter->HasAuthority())) return;
	if (InNewOuter == GetOuter()) return;
	Rename(nullptr, InNewOuter);
}

#pragma endregion TOOLS

