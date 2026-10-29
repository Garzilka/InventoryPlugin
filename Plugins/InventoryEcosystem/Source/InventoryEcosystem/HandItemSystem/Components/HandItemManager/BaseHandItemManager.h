// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComboManagerComponent.h"
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/HandItemSystem/Data/HandItemData.h"
#include "Components/ActorComponent.h"
#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"
#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "BaseHandItemManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHandItemManagerUpdateSignature, class UBaseHandItemManager*, ManagerRef);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNewCurrentSlotSignature, class UBaseHandItemManager*, ManagerRef, AActor*, CurrentActor, int32, NewCurrentSlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories = ("Variable", "Sockets", "Coocking", "Replication", "ComponentReplication", "ComponentTick", "Cooking", "Physics", "Navigation", "Animation", "AssetUserData", "Collision"))
class HANDITEMSYSTEM_API UBaseHandItemManager : public UComboManagerComponent, public IItemManagmentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FHandItemManagerSave SavedData;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FHandItemManagerUpdateSignature OnHandItemManagerUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FHandItemManagerUpdateSignature OnSlotsUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FNewCurrentSlotSignature OnNewCurrentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Slots)
	FHandSlots ItemData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumSlots = 9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AlwaysSlotOppened;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_State)
	FHandItemState HandItemState;

	UPROPERTY()
	FTimerHandle Timer_InterpolateFovTo;

	UPROPERTY()
	FTimerHandle Timer_InterpolateFovFrom;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsAIM = false;

	float PreviewTimeFOV = 0;

	UPROPERTY(BlueprintReadOnly)
	bool IsPlayEquipMontagne = false;

	bool IsSerialized = false;
	bool IsLoaded = false;
public:	
	UBaseHandItemManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Ищет менеджер предметов в объекте */
	UFUNCTION(BlueprintCallable, Category = "HandItemManager|Tools")
	static UBaseHandItemManager* GetBaseHandItemManager(AActor* FromActor);

	UFUNCTION(BlueprintCallable)
	void SwitchScope(bool bUse);

	UFUNCTION(BlueprintCallable)
	void FirstAction(bool bUse);

	UFUNCTION(BlueprintCallable)
	void SecondAction(bool bUse);

	UFUNCTION(BlueprintCallable)
	void MeleeAction();

	UFUNCTION(BlueprintCallable)
	void ReloadAction();

	UFUNCTION(BlueprintCallable)
	bool EquipItem(int32 Slot);

	UFUNCTION(BlueprintCallable)
	bool UnequipCurrentActor();

	UFUNCTION(BlueprintCallable)
	bool SetupItem(UBaseItem* Target, int32 Slot);

	UFUNCTION(BlueprintCallable)
	bool RemoveItemByItem(UBaseItem* Target);

	UFUNCTION(BlueprintPure)
	UBaseItem* GetItemAtIndex(int32 Idx);

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetItemActorAtIndex(int32 Idx);

	UFUNCTION(BlueprintCallable)
	void NotifyEquip(EEquipNotifyType NotifyType);

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetCurrentActor();

	UFUNCTION(BlueprintPure)
	FHandItemState GetHandItemState();

	UFUNCTION(BlueprintPure)
	bool InActiveAction();

	UFUNCTION(BlueprintPure)
	int32 GetSlotIdByTag(FGameplayTag InTag);

	UFUNCTION(BlueprintPure)
	int32 GetSlotIdByItem(UBaseItem* ItemRef);

	UFUNCTION(BlueprintPure)
	int32 GetSlotIdByItemActor(ABaseItemActor* ItemRef);

	UFUNCTION(BlueprintPure)
	TArray<FHandSlotSetting> GetItemData() { return ItemData.Slots; }

	UFUNCTION(BlueprintPure)
	TArray<FHandSlotSetting> GetAllItem() { return ItemData.Slots; }

protected:
	virtual void BeginPlay() override;
	virtual void Serialize(FArchive& Ar) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void LoadData();
	
	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_State();

	UFUNCTION(Reliable, Server)
	void Server_IsAIM(bool NewState);
	void Server_IsAIM_Implementation(bool NewState) { bIsAIM = NewState; }

	UFUNCTION(Reliable, Server)
	void Server_EquipItem(int32 InTargetSlot);
	void Server_EquipItem_Implementation(int32 InTargetSlot);

	UFUNCTION(Reliable, NetMulticast)
	void NetMulticast_EquipItem(UAnimMontage* Animation);
	void NetMulticast_EquipItem_Implementation(UAnimMontage* Animation);

	UFUNCTION(Reliable, Server)
	void Server_SetupItem(UBaseItem* Target, int32 Slot);
	void Server_SetupItem_Implementation(UBaseItem* Target, int32 Slot);



	bool CanSerialize()
	{
		if ((!GEngine || !GetOwner() || HasAnyFlags(RF_ArchetypeObject)))
			return false;
		if ((GetWorld() && GetWorld()->WorldType == EWorldType::EditorPreview))
			return false;
		if (!HasAuthority())
			return false;
		return true;
	};
	UFUNCTION()
	void ItemDestroyed(AActor* ItemRef);

	UFUNCTION()
	void InterpolateFOVTo();

	UFUNCTION()
	void InterpolateFOVFrom();

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(UCoreInventoryObject* Item);
	virtual bool I_RemoveItem_Implementation(class UCoreInventoryObject* Item) { return RemoveItemByItem(Cast<UBaseItem>(Item)); }

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(UCoreInventoryObject* Item);
	virtual bool I_CanRemoveItem_Implementation(UCoreInventoryObject* Item) { return true; }
};
