// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/InteractSystem/Actor/BaseInteractedActor.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "BaseItemActor.generated.h"

class UBaseInteractComponent;
class UMeshComponent;

UCLASS(BlueprintType, HideCategories = ("Actor Tick", "Rendering", "Replication", "Input", "Actor", "Base", "Collision", "Shape", "MeshComponent", "Cooking", "LOD", "WorldPartition"))
class ITEMSYSTEM_API ABaseItemActor : public ABaseInteractedActor, public IItemManagmentInterface
{
	GENERATED_BODY()

	friend class ABaseItemActor;
protected:

	UPROPERTY(ReplicatedUsing = OnRep_Item)
	class UBaseItem* ItemRef = nullptr;

	UPROPERTY(EditAnywhere)
	FMakeItemContext ItemContext;
	
	bool DoNeedRespawnOnBeginPlay = true;

	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;
public:	
	ABaseItemActor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	UBaseItem* GetItem() const;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	bool SetupItem(UBaseItem* NewItem, bool NeedSwitchParren = true);
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(UCoreInventoryObject* InObject);
	virtual bool I_RemoveItem_Implementation(UCoreInventoryObject* InObject);

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(UCoreInventoryObject* InObject);
	virtual bool I_CanRemoveItem_Implementation(UCoreInventoryObject* InObject) { return true; }
	
	UFUNCTION()
	void Event_Interacted(APawn* InCharacterInstigator, UBaseInteractComponent* InInteractComponent, EInteractType InInteractType);
	
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void SpawnItem();

	void LoadMesh(bool DoForce = false);
	
	UFUNCTION()
	void OnRep_Item();

	UFUNCTION()
	virtual void Async_LoadStaticMesh(FSoftObjectPath Reference);

	UFUNCTION()
	void Async_LoadSkeletalMesh(FSoftObjectPath Reference);
};
