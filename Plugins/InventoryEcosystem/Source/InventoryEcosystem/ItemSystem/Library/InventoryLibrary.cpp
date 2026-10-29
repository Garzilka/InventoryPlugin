// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryLibrary.h"

#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Public/ItemSettings.h"
#include "InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h"
#include "InventoryEcosystem/InteractSystem/InteractSystemSettings.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"


UBaseItem* UInventoryLibrary::MakeItem(AActor* Owner, FMakeItemContext MakeItemContext)
{
    if (!MakeItemContext.bIsCustom)
    {
        return UInventoryLibrary::MakeItem(Owner, MakeItemContext.ItemData);
    }
    UMakeItemRule* MakeItemRule = NewObject<UMakeItemRule>(Owner, MakeItemContext.MakeItemRule);
    if (!UKismetSystemLibrary::IsValidClass(MakeItemRule->GetItemClass())) return nullptr;
    
    UBaseItem* Result = NewObject<UBaseItem>(Owner, MakeItemRule->GetItemClass());
    Result->BeginPlay();
    
    MakeItemRule->MakeItem(Result);
    return Result;
}

UBaseItem* UInventoryLibrary::MakeItem(AActor* Owner, FInventoryItemData Item)
{
	if (!UKismetSystemLibrary::IsValidClass(Item.ItemClass)) return nullptr;
	UBaseItem* L_Item = NewObject<UBaseItem>(Owner, Item.ItemClass);
	L_Item->BeginPlay();
	return L_Item;
}

TArray<UBaseItem*> UInventoryLibrary::MakeItems(AActor* Owner, FInventoryItemData Data)
{
	if (!Owner) return {};
	if (!Owner && !Owner->HasAuthority() && !UKismetSystemLibrary::IsValidClass(Data.ItemClass)) return TArray<UBaseItem*>();

	if (!UKismetSystemLibrary::IsValidClass(Data.ItemClass)) return TArray<UBaseItem*>();

	TArray<UBaseItem*> LResult;
	UBaseItem* LItem = nullptr;
	int32 LTempAmmout = (Data.Range == FIntPoint(-1) ?  Data.Amount : FMath::RandRange(Data.Range.GetMin(), Data.Range.GetMax()));
	bool IsStackable = Cast<UBaseItem>(Data.ItemClass->GetDefaultObject())->IsStackable();
	while (LTempAmmout > 0)
	{
		LItem = NewObject<UBaseItem>(Owner, Data.ItemClass);
		if (IsStackable)
		{

			if (LItem->GetStackSize() > LTempAmmout)
			{
				LItem->SetAmount(LTempAmmout);
				LTempAmmout = -1;
			}
			else
			{
				LItem->SetAmount(LItem->GetStackSize());
				LTempAmmout -= LItem->GetStackSize();
			}
		}
		else
		{
			LTempAmmout--;
		}
		LResult.Add(LItem);
	}
	return LResult;
}

TArray<UBaseItem*> UInventoryLibrary::MakeItems(AActor* Owner, FMakeItemContext Data)
{
	if (!Owner) return {};
	if (!Owner && !Owner->HasAuthority() && !UKismetSystemLibrary::IsValidClass(Data.ItemData.ItemClass)) return TArray<UBaseItem*>();

	if (Data.bIsCustom)
	{
	    return { UInventoryLibrary::MakeItem(Owner, Data) };
	}

	
	if (!UKismetSystemLibrary::IsValidClass(Data.ItemData.ItemClass)) return TArray<UBaseItem*>();

	TArray<UBaseItem*> LResult;
	UBaseItem* LItem = nullptr;
	int32 LTempAmmout = (Data.ItemData.Range == FIntPoint(-1) ?  Data.ItemData.Amount : FMath::RandRange(Data.ItemData.Range.GetMin(), Data.ItemData.Range.GetMax()));
	bool IsStackable = Cast<UBaseItem>(Data.ItemData.ItemClass->GetDefaultObject())->IsStackable();
	while (LTempAmmout > 0)
	{
		LItem = NewObject<UBaseItem>(Owner, Data.ItemData.ItemClass);
		if (IsStackable)
		{

			if (LItem->GetStackSize() > LTempAmmout)
			{
				LItem->SetAmount(LTempAmmout);
				LTempAmmout = -1;
			}
			else
			{
				LItem->SetAmount(LItem->GetStackSize());
				LTempAmmout -= LItem->GetStackSize();
			}
		}
		else
		{
			LTempAmmout--;
		}
		LResult.Add(LItem);
		LItem->BeginPlay();
	}
	return LResult;
}

UBaseItem* FSpawnItemContext::GetItem() const
{
	if (Item) return Item;

	return UInventoryLibrary::MakeItem(NewOwner, ItemData);
}

ABaseItemActor* UInventoryLibrary::SpawnItemObjectContext(const UObject* WorldContextObject, FSpawnItemContext InSpawnItemContext)
{
	if (!(WorldContextObject && InSpawnItemContext.IsDataValid())) return nullptr;

	//GetSpawn class or set by default
	UClass* ClassToSpawn = (UKismetSystemLibrary::IsValidClass(InSpawnItemContext.ClassToSpawn) ? InSpawnItemContext.ClassToSpawn.Get() : ABaseItemActor::StaticClass());
	if (!UKismetSystemLibrary::IsValidClass(ClassToSpawn)) return nullptr;

	ABaseItemActor* LNewItemObject = WorldContextObject->GetWorld()->SpawnActorDeferred<ABaseItemActor>(ClassToSpawn, InSpawnItemContext.SpawnLocation, InSpawnItemContext.NewOwner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!IsValid(LNewItemObject)) return nullptr;

	LNewItemObject->bOnlyRelevantToOwner = InSpawnItemContext.bReplicateOnlyOwner;
	LNewItemObject->SetOwner(InSpawnItemContext.NewOwner);

	if (LNewItemObject->SetupItem(InSpawnItemContext.GetItem(), InSpawnItemContext.WithSwitchParrent))
	{
		if (InSpawnItemContext.bWithSpawnFinish)
		{
			LNewItemObject->FinishSpawning(InSpawnItemContext.SpawnLocation);
		}
	}
	else
	{
		LNewItemObject->Destroy();
		return nullptr;
	}

	return  LNewItemObject;
}

float UInventoryLibrary::GetItemCellSize()
{
	const UItemSettings* Settings = GetDefault<UItemSettings>();
	return (Settings != nullptr ? Settings->ItemCellSize : 50.f);
}

FName UInventoryLibrary::GetDefaultInteractProfile()
{
	const UInteractSystemSettings* Settings = UInteractSystemSettings::Get();
	return (Settings != nullptr ? Settings->InteractProfile : FName("Interacted"));
}
