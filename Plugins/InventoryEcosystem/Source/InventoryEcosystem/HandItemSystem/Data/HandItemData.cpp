// Fill out your copyright notice in the Description page of Project Settings.


#include "HandItemData.h"

#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/BaseHandItemManager.h"
#include "Serialization/MemoryWriter.h"

HandItemData::HandItemData()
{
}

HandItemData::~HandItemData()
{
}
FHandItemManagerSlotSave::FHandItemManagerSlotSave(UBaseItem* Item, FGameplayTag InSlotTag)
{
	SlotTag = InSlotTag;
	if (!IsValid(Item)) return;
	FMemoryWriter MemoryWriter(ItemData, true);
	FSaveGameArchive ArSave(MemoryWriter);
	Item->Serialize(ArSave);
	SavedClass = Item->GetClass();
}

FHandItemManagerSave::FHandItemManagerSave(UBaseHandItemManager* HandItemManager)
{
	Saved = 1;
	SavedData.Reset();

	for (auto ItemData : HandItemManager->GetItemData())
	{
		SavedData.Add(FHandItemManagerSlotSave(ItemData.ItemRef, ItemData.SlotType));
	}
}