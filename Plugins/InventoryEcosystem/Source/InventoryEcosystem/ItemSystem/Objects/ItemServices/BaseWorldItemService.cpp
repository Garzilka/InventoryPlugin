// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWorldItemService.h"

#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"


bool UBaseWorldItemService::SetSpawnedRef(ABaseItemActor* NewReference)
{
	SpawnedReference = NewReference;

	//Blocking a double delegate call
	if (IsSpawnProcess) return true;
	
	OnPreSpawnItem.Broadcast(this, SpawnedReference);
	OnSpawnItem.Broadcast(this, SpawnedReference);

	return true;
}

ABaseItemActor* UBaseWorldItemService::SpawnItem(FSpawnWorldItemServiceContext Context)
{
	if (IsValid(SpawnedReference)) return SpawnedReference;

	IsSpawnProcess = true;
	FSpawnItemContext ItemContext(GetItemOwner(), GetItemOwner()->GetOwner(), Context.GetTransform(), CustomClass);
	ItemContext.WithSwitchParrent = Context.IsWithSwitchParrent();
	ItemContext.bWithSpawnFinish = false;

	SpawnedReference = UInventoryLibrary::SpawnItemObjectContext(GetItemOwner()->GetOwner()->GetWorld(), ItemContext);
	if (!SpawnedReference)
	{
		IsSpawnProcess = false;
		return nullptr;
	}

	OnPreSpawnItem.Broadcast(this, SpawnedReference);

	SpawnedReference->FinishSpawning(ItemContext.SpawnLocation);
	SpawnedReference->OnDestroyed.AddUniqueDynamic(this, &UBaseWorldItemService::DestroyActor);

	OnSpawnItem.Broadcast(this, SpawnedReference);	
	IsSpawnProcess = false;
	
	return SpawnedReference;
}

void UBaseWorldItemService::DestroyFromWorld()
{
	if (!IsValid(SpawnedReference)) return;
	SpawnedReference->Destroy();
	SpawnedReference = nullptr;
}

void UBaseWorldItemService::Server_DropToWorld_Implementation(ACharacter* At)
{
	/*FSpawnWorldItemServiceContext ItemServiceContext;
	ItemServiceContext.ForAI = false;
	ItemServiceContext.Position.SetLocation(UInventoryLibrary::FindPositionForSpawn(At));
	ItemServiceContext.WithSwitchParrent = true;
	auto Result = SpawnItem(ItemServiceContext);
	if (!Result) return;
	Result->SetLifeSpan(1800.f);*/
}

void UBaseWorldItemService::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseWorldItemService, SpawnedReference);
}


void UBaseWorldItemService::BeginPlay(UBaseItem* Item)
{
	Super::BeginPlay(Item);
}

void UBaseWorldItemService::OnRep_SpawnedReference()
{
}

void UBaseWorldItemService::DestroyActor(AActor* ItemActor)
{
	if (SpawnedReference != ItemActor) return;
	SpawnedReference = nullptr;
}