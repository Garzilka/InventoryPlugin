// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInventoryItemService.h"

#include "InventoryEcosystem/InventoryTools/Data/SaveGameData.h"
#include "InventoryEcosystem/InventoryManagerSystem/Component/BaseInventoryManager.h"

#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"

FInventoryManagerSlotSave::FInventoryManagerSlotSave(UBaseInventoryItemService* Data)
{
    if (!(Data && Data->GetInventoryRef())) return;
	
    IsSaved = true;
    InventoryData.Reset();

    FMemoryWriter MemoryWriter(InventoryData, true);
    FSaveGameArchive ArSave(MemoryWriter);
    Data->GetInventoryRef()->Serialize(ArSave);
}

void UBaseInventoryItemService::BeginPlay(UBaseItem* Item)
{
    Super::BeginPlay(Item);
    Item->OnItemEquiped.AddUniqueDynamic(this, &UBaseInventoryItemService::Event_ItemEquiped);
    Item->OnItemUnequiped.AddUniqueDynamic(this, &UBaseInventoryItemService::Event_ItemUnequiped);
}

void UBaseInventoryItemService::Serialize(FArchive& Ar)
{
    if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }
    
    if (Ar.IsSaving())
    {
    	SavedData = FInventoryManagerSlotSave(this);
    	Super::Serialize(Ar);
    	Ar << SavedData;
    	return;
    }
    
    Super::Serialize(Ar);
    Ar << SavedData;
    
    
	if (SavedData.IsSaved && GetInventoryRef())
	{
		FMemoryReader MemoryReader(SavedData.InventoryData, true);
		FSaveGameArchive ArLoad(MemoryReader);
		GetInventoryRef()->Serialize(ArLoad);
	}
}

UBaseLineInventory* UBaseInventoryItemService::MakeInventory(AActor* InventoryOwner)
{
    if (!GetItemOwner()) return nullptr;
    if (GetInventoryRef()) return GetInventoryRef();

    AActor* LPossibleOwner = InventoryOwner;
    if (!LPossibleOwner)
    {
        LPossibleOwner = GetItemOwner()->GetOwningPlayer();
    }
    if (!LPossibleOwner) return nullptr;

    InventoryRef = NewObject<UBaseLineInventory>(InventoryOwner, InventoryClass);
    if (!InventoryRef) return nullptr;
	

    //todo: LoadSafeData
    if (SavedData.IsSaved &&  GetInventoryRef())
    {
        FMemoryReader MemoryReader(SavedData.InventoryData, true);
        FSaveGameArchive ArLoad(MemoryReader);
        GetInventoryRef()->Serialize(ArLoad);
        InventoryRef->RegisterComponentWithWorld(GetWorld());
    }
    else
    {
        TArray<UBaseItem*> AllItems;
        InventoryRef->RegisterComponentWithWorld(GetWorld());
        InventoryRef->SetSize(SizeInventory, AllItems);
    }
	
    return InventoryRef;
}


void UBaseInventoryItemService::DestroyAndSafeInventory()
{
    if (!IsValid(InventoryRef)) return;

    SafeInventoryState();
	
    InventoryRef->MarkAsGarbage();
    InventoryRef = nullptr;
}

void UBaseInventoryItemService::SafeInventoryState()
{
	//todo: Safe inventory data;
}

void UBaseInventoryItemService::Event_ItemEquiped(UBaseItem* ItemRef)
{
    UBaseInventoryManager* InvManager = UInventoryToolsLibrary::GetComponentFromOwner<UBaseInventoryManager>(GetOwner(), UBaseInventoryManager::StaticClass());
    if (!InvManager) return;
    InvManager->AddInventory(GetItemOwner());
}

void UBaseInventoryItemService::Event_ItemUnequiped(UBaseItem* ItemRef)
{
    UBaseInventoryManager* InvManager = UInventoryToolsLibrary::GetComponentFromOwner<UBaseInventoryManager>(GetOwner(), UBaseInventoryManager::StaticClass());
    if (!InvManager) return;
    SafeInventoryState();
    InvManager->RemoveInventory(GetItemOwner());
}
