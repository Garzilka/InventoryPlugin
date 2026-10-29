// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponModuleService.h"

#include "BaseWeaponService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BasePickupService.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseWeapon.h"

#pragma region ENGINE

void UBaseWeaponModuleService::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseWeaponModuleService, InstalTo);
	DOREPLIFETIME(UBaseWeaponModuleService, InstalPlace);
	DOREPLIFETIME(UBaseWeaponModuleService, WeaponOwner);
	DOREPLIFETIME(UBaseWeaponModuleService, ModificationData);
}

#pragma endregion ENGINE

#pragma region SAVE/LOAD

void UBaseWeaponModuleService::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }

	if (Ar.IsSaving())
	{
		SaveItemData.Reset();
		for (auto& Module : ModificationData)
		{
			if (!(IsValid(Module.AttachedModule) && IsValid(Module.AttachedModule->GetItemOwner()))) continue;
			FWeaponModuleServiceSave NewData;
			NewData.SaveItem = Module.AttachedModule->GetItemOwner()->GetClass();
			NewData.SaveTag = Module.AttachedModule->InstalTo.UID;
			SaveItemData.Add(NewData);
		}
		Super::Serialize(Ar);

		for (auto Module : ModificationData)
		{
			if (!(IsValid(Module.AttachedModule) && IsValid(Module.AttachedModule->GetItemOwner()))) continue;
			Module.AttachedModule->GetItemOwner()->Serialize(Ar);
		}

		return;
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		for (auto SaveData : SaveItemData)
		{
			FInventoryItemData ItemData;
			ItemData.Amount = 1;
			ItemData.ItemClass = SaveData.SaveItem;
			UBaseItem* Item = UInventoryLibrary::MakeItem(GetOwner(), ItemData);
			if (!Item) continue;
			for (auto& Module : ModificationData)
			{
				if (Module.UID != SaveData.SaveTag) continue;
				AttachItem(Item, Module);
				break;
			}
			Item->Serialize(Ar);
		}
	}
}

// TArray<UObject*> UBaseWeaponModuleService::GetAllChieldObject()
// {
// 	TArray<UObject*> Result;
// 	for (auto Service : AttachedModules)
// 	{
// 		if (!IsValid(Service)) continue;
// 		Result.Add(Service->GetItemOwner());
// 		Result.Append(Service->GetItemOwner()->GetAttachedSubObject());
// 	}
// 	return Result;
// }
// TArray<UBaseItem*> UBaseWeaponModuleService::GetAttachedItem()
// {
// 	TArray<UBaseItem*> Result;
// 	for (auto Service : AttachedModules)
// 	{
// 		if (!IsValid(Service)) continue;
// 		Result.Add(Service->GetItemOwner());
// 		Result.Append(Service->GetItemOwner()->GetAttachedItem());
// 	}
// 	return Result;
// }

#pragma endregion //SAVE/LOAD

#pragma region ATTACH

bool UBaseWeaponModuleService::AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate)
{
	if (!IsValid(Item)) return false;

	UBaseWeaponModuleService* Service = Item->GetServiceTyped<UBaseWeaponModuleService>(UBaseWeaponModuleService::StaticClass());
	if (!IsValid(Service)) return false;
	
	int32 Idx = ModificationData.Find(TargetPlacement);
	if  (Idx == INDEX_NONE) return false;
	
	FWeaponModuleInfo ModData;
	if (!Service->GetModificationInfoItem(GetItemOwner()->GetClass(), ModData)) return false;

	if (!Item->HasAuthority())
	{
		Server_AttachItem(Item, TargetPlacement, NeedCreate);
		return true;
	}

	Service->InstalTo = TargetPlacement;
	Service->WeaponOwner = WeaponOwner;
	Service->InstalPlace = this;
	Service->OnWeaponModuleServiceUpdate.AddUniqueDynamic(this, &UBaseWeaponModuleService::Event_UpdateModule);
	
	if (WeaponOwner)
	{
		for (auto SocketUID : Service->GetBlockSlotBySocketUID())
		{
			WeaponOwner->AddBlockSocket(SocketUID);
		}
	}
	
	if (auto Module = ModificationData[Idx].AttachedModule)
	{
		if (Module->GetItemOwner() && Module->GetItemOwner()->GetPickupService())
		{
			Module->GetItemOwner()->GetPickupService()->PickupItem(GetOwningPlayerPawn());
		}
	}
	ModificationData[Idx].AttachedModule = Service;
	Item->SwitchParrent(this);

	if (WeaponOwner)
	{
		if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(WeaponOwner->GetSpawnedRef()))
		{
			WeaponRef->AttachModule(Service, TargetPlacement);
		}
	}

	OnRep_AttachedModules();
	return true;
}

void UBaseWeaponModuleService::Server_AttachItem_Implementation(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate)
{
	if (!Item->HasAuthority()) return;
	AttachItem(Item, TargetPlacement, NeedCreate);
}

#pragma endregion ATTACH


#pragma region DETACH

bool UBaseWeaponModuleService::DetachItem(UBaseItem* Module)
{
	if (!IsValid(Module)) return false;
	UBaseWeaponModuleService* Service = Module->GetServiceTyped<UBaseWeaponModuleService>(UBaseWeaponModuleService::StaticClass());

	int32 Idx = INDEX_NONE;
	for (int32 LIdx = 0; LIdx < ModificationData.Num(); LIdx++)
	{
		if (!(ModificationData[LIdx].AttachedModule && ModificationData[LIdx].AttachedModule->GetItemOwner())) continue;
		
		if (ModificationData[LIdx].AttachedModule->GetItemOwner() != Module) continue;
		Idx = LIdx;
		break;
	}
	if  (Idx == INDEX_NONE) return true;

	if (WeaponOwner)
	{
		if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(WeaponOwner->GetSpawnedRef()))
		{
			WeaponRef->DetachModule(Service);
		}
	}
	if (WeaponOwner)
	{
		for (auto SocketUID : Service->GetBlockSlotBySocketUID())
		{
			WeaponOwner->RemoveBlockSocket(SocketUID);
		}
	}
	if (Service->OnWeaponModuleServiceUpdate.IsAlreadyBound(this, &UBaseWeaponModuleService::Event_UpdateModule))
		Service->OnWeaponModuleServiceUpdate.RemoveDynamic(this, &UBaseWeaponModuleService::Event_UpdateModule);

	auto LItem = ModificationData[Idx].AttachedModule->GetItemOwner();
	ModificationData[Idx].AttachedModule = nullptr;
	if (LItem->GetPickupService())
	{
		Service->DetachAllAttaches();
		LItem->GetPickupService()->PickupItem(GetOwningPlayerPawn());
	}

	OnRep_AttachedModules();

	return true;
}

void UBaseWeaponModuleService::DetachBySocketUID(FName SocketUID)
{
	for (auto& Module : ModificationData)
	{
		if (!Module.AttachedModule) continue;
		if (Module.UID != SocketUID) continue;
		Module.AttachedModule->DetachBySocketUID(SocketUID);
		DetachItem(Module.AttachedModule->GetItemOwner());
	}
}

void UBaseWeaponModuleService::DetachAllAttaches()
{
	for (auto& Module : ModificationData)
	{
		if (!(Module.AttachedModule && Module.AttachedModule->GetItemOwner())) continue;
		
		DetachItem(Module.AttachedModule->GetItemOwner());
	}
}

TArray<UBaseWeaponModuleService*> UBaseWeaponModuleService::GetAttachedModules(bool OnlySelf)
{
	TArray<UBaseWeaponModuleService*> Result;
	if(OnlySelf)
	{
		for (auto& Module : ModificationData)
		{
			if (!Module.AttachedModule) continue;
			Result.Add(Module.AttachedModule);
		}
		return Result;
	}

	for (auto& Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		Result.Add(Module.AttachedModule);
		Result.Append(Module.AttachedModule->GetAttachedModules(OnlySelf));
	}
	return Result;
}

#pragma endregion DETACH

#pragma region TOOLS

bool UBaseWeaponModuleService::GetModificationInfoItem(TSubclassOf<UBaseItem> ByWeaponItem, FWeaponModuleInfo& Info)
{
	auto LData = ModItemInfo.Find(ByWeaponItem);
	if (!LData) return false;
	Info = *LData;
	return true;
}

TArray<UBaseItem*> UBaseWeaponModuleService::GetModificationsByTag(FGameplayTag Tag)
{
	TArray<UBaseItem*> Result;
	for (auto& Module : ModificationData)
	{
		if (!IsValid(Module.AttachedModule)) continue;
		if (!(Tag == Module.AttachTag || Tag == Module.AttachedModule->InstalTo.AttachTag)) continue;
		Result.Add(Module.AttachedModule->GetItemOwner());
	}
	return Result;
}

UBaseItem* UBaseWeaponModuleService::GetModificationBySocket(FWeaponSocketInfo SocketInfo)
{
	int32 Idx = ModificationData.Find(SocketInfo);
	if (Idx == INDEX_NONE) return nullptr;
	return (IsValid(ModificationData[Idx].AttachedModule) ? ModificationData[Idx].AttachedModule->GetItemOwner() : nullptr);
}

void UBaseWeaponModuleService::RefreshAttachment()
{
    if (!WeaponOwner) return;
    
    if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(WeaponOwner->GetSpawnedRef()))
    {
        for (auto& Module : ModificationData)
        {
            if (!Module.AttachedModule) continue;
				
            WeaponRef->AttachModule(Module.AttachedModule, Module.AttachedModule->InstalTo);
            Module.AttachedModule->WeaponOwner = WeaponOwner;
            Module.AttachedModule->RefreshAttachment();
        }
    }
}

#pragma endregion TOOLS


void UBaseWeaponModuleService::Event_UpdateModule(UBaseWeaponModuleService* ModuleRef)
{
	OnRep_AttachedModules();
}

void UBaseWeaponModuleService::OnRep_AttachedModules()
{
	if (WeaponOwner)
	{
		if (ABaseWeapon* WeaponRef = Cast<ABaseWeapon>(WeaponOwner->GetSpawnedRef()))
		{
			WeaponRef->RefreshAttachment();
		}
	}
	OnWeaponModuleServiceUpdate.Broadcast(this);
}