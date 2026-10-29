// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMeshData.h"

#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/EquipManager/Component/BaseEquipManager.h"

FEquipManagerSave::FEquipManagerSave(UBaseEquipManager* EquipManager)
{
	Saved++;
	SaveData.Reset();

	for (auto Item : EquipManager->GetAllItem())
	{
		if (!Item) continue;
		FEquipManagerSlotSave NewData = FEquipManagerSlotSave(Item);
		SaveData.Add(NewData);
	}
}

FEquipManagerSlotSave::FEquipManagerSlotSave(UBaseItem* Item)
{
	if (!IsValid(Item)) return;

	FMemoryWriter MemoryWriter(ItemData, true);
	FSaveGameArchive ArSave(MemoryWriter);
	Item->Serialize(ArSave);
	SavedClass = Item->GetClass();
}