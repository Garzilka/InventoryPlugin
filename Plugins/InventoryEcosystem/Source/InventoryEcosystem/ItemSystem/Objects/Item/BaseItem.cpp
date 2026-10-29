// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BasePickupService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseQualityService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseWorldItemService.h"
#include "UObject/AssetRegistryTagsContext.h"

void UBaseItem::Serialize(FArchive& Ar)
{
	if (!CanSerialize() || !Ar.IsSaveGame()) { Super::Serialize(Ar); return; }
	
	if (Ar.IsSaving())
	{
		Super::Serialize(Ar);
		for (auto Service : Services)
		{
			if (!IsValid(Service)) continue;
			Service->Serialize(Ar);
		}
		return;
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		for (auto Service : Services)
		{
			if (!IsValid(Service)) continue;
			Service->Serialize(Ar);
		}
	}
}

void UBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseItem, bIsRotate);
	DOREPLIFETIME(UBaseItem, Amount);
	DOREPLIFETIME(UBaseItem, PickupService);
	DOREPLIFETIME(UBaseItem, WorldPlacementService);
	DOREPLIFETIME(UBaseItem, QualityService);
	DOREPLIFETIME(UBaseItem, Services);
}

void UBaseItem::GetServiceTyped(TSubclassOf<UBaseItemService> ClassService, UBaseItemService*& Service, bool CheckChild)
{
	if (!UKismetSystemLibrary::IsValidClass(ClassService))
	{
		Service = nullptr;
		return;
	}

	for (UBaseItemService* Result : Services)
	{
		if (!IsValid(Result)) continue;

		if (CheckChild ? Result->IsA(ClassService) : (ClassService->GetClass() == ClassService))
		{
			Service = Result;
			return;
		}
	}

	Service = nullptr;
}


void UBaseItem::BeginPlay()
{
	if (IsValid(PickupService))
	{
		PickupService->BeginPlay(this);
	}
	if (IsValid(WorldPlacementService))
	{
		WorldPlacementService->BeginPlay(this);
	}

	if (IsValid(QualityService))
	{
		QualityService->BeginPlay(this);
	}

	for (UBaseItemService* LService : Services)
	{
		if (!IsValid(LService)) continue;
		LService->BeginPlay(this);
	}
}

float UBaseItem::GetItemWeight(bool IsFull)
{
	float Result = DefaultSettings.Weight;
	if (!IsFull) return Result;

	for (auto Item : GetAttachedItem())
	{
		Result += Item->GetItemWeight(false);
	}

	return Result;
}

void UBaseItem::SetIcon(UTexture* Texture)
{
	DefaultSettings.Icon = Texture;
	OnItemUpdate.Broadcast(this);
}

TArray<UObject*> UBaseItem::GetAttachedSubObject()
{
	TArray<UObject*> Result = { PickupService, WorldPlacementService, QualityService };

	for (auto Service : Services)
	{
		if (!Service) continue;

		Result.Add(Service);
		Result.Append(Service->GetAttachedSubObject());
	}

	return Result;
}

void UBaseItem::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
    Super::GetAssetRegistryTags(Context);
    if (DefaultSettings.Icon)
    {
        // Преобразуем ссылку на текстуру в строку (путь)
        FString IconPath = DefaultSettings.Icon->GetPathName();
        
        // Добавляем тэг "ItemIconPath" в метаданные ассета
        Context.AddTag(FAssetRegistryTag(TEXT("ItemIconPath"), IconPath, FAssetRegistryTag::TT_Alphabetical));
    }
}

void UBaseItem::SetRender(UTextureRenderTarget2D *InRender)
{
    Render = InRender;
    IsValidRender = true;
    OnCaptureUpdate.Broadcast(this);
    OnItemUpdate.Broadcast(this);
}

TArray<UBaseItem*> UBaseItem::GetAttachedItem(bool IncludedSelf)
{
	TArray<UBaseItem*> Result = (IncludedSelf ? TArray<UBaseItem*>{ this } : TArray<UBaseItem*>{} );

	for (auto Service : Services)
	{
		if (!Service) continue;
		Result.Append(Service->GetAttachedItem());
	}

	return Result;
}

/*
* ���������� ���������� ������� �� �����������
*/
int32 UBaseItem::SetAmount(int32 NewAmmout)
{
	if (!HasAuthority()) return -1;

	Amount = NewAmmout;
	if (GetStackSize() >= Amount) return 0;
	Amount = GetStackSize();
	OnRep_Amount();
	return (NewAmmout - GetStackSize());
}

bool UBaseItem::StackItems(UBaseItem* TargetItem)
{
	if (!CanBeStack(TargetItem)) return false;

	int32 Remain = SetAmount(GetAmount() + TargetItem->GetAmount());
	if (Remain < 1)
	{
		TargetItem->RemoveFromParrent();
		TargetItem->MarkAsGarbage();
	}
	else
	{
		TargetItem->SetAmount(Remain);
	}
	OnRep_Amount();
	return true;
}

bool UBaseItem::CanBeStack(UBaseItem* TargetItem)
{
	if (!(DefaultSettings.bIsStack && IsValid(TargetItem))) return false;
	if (GetClass() != TargetItem->GetClass()) return false;
	return true;
}

int32 UBaseItem::SubstructAmount(int32 InAmount)
{
	if (!HasAuthority()) return -1;
	Amount -= InAmount;
	return Amount;
}