// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterEditor.h"
#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "BaseEquipManager.generated.h"

#define LOG_EQUIPMANAGER(type, text) UInventoryLOG::PrintInventoryLog(GetOwner(), type, text, __LINE__, __FUNCTION__)

/** Эквип менеджер обновился */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateEquipmentSignature, class UBaseEquipManager*, EquipManagerRef);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateArmorDataSignature, class UBaseEquipManager*, EquipManagerRef);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipSlotsUpdateSignature, class UBaseEquipManager*, EquipManagerRef, FGameplayTagContainer, AvailableEquipSlots);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories = ("Variable", "Sockets", "Coocking", "Replication", "ComponentReplication", "ComponentTick", "Cooking", "Physics", "Navigation", "Animation", "AssetUserData", "Collision"))
class EQUIPMANAGER_API UBaseEquipManager : public UBaseCharacterEditor, public IItemManagmentInterface
{
	GENERATED_BODY()

public:
	/** Вызовется когда обновится массив, исключая изменения внутри предметов */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateEquipmentSignature OnUpdateEquipment;

	/** Вызовется когда обновится массив, исключая изменения внутри предметов */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateArmorDataSignature OnBodyArmorDataUpdate;

	/** Вызовется когда обновится массив, исключая изменения внутри предметов */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateArmorDataSignature OnClothArmorDataUpdate;

	/** Вызовется когда обновится массив, исключая изменения внутри предметов */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateArmorDataSignature OnArmorDataUpdate;

	/** Вызовется когда обновится массив, исключая изменения внутри предметов */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FEquipSlotsUpdateSignature OnEquipSlotsUpdate;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_EquipArray)
	TArray<UBaseItem*> EquipArray;

	UPROPERTY(EditDefaultsOnly, Category = "Настройки|MeshManager|Предметы", meta = (AllowPrivateAccess = "true", DisplayName = "Предметы по умолчанию", ForceInlineRow))
	TMap<FGameplayTag, TSubclassOf<UBaseItem>> DefaultItem;

	FTimerHandle RetriggerableDelay_RecalculateBody;
	
	UPROPERTY()
	FEquipManagerSave SavedData;
public:	
	UBaseEquipManager();

	FGameplayTagContainer AvailableSlots;

	UFUNCTION(BlueprintCallable)
	virtual bool SetClothItem(UBaseItem* TargetItem);

	UFUNCTION(BlueprintCallable)
	virtual bool RemoveItemByItem(UBaseItem* TargetItem);

	UFUNCTION(BlueprintPure)
	UBaseItem* GetClothItemByTag(FGameplayTag ClothTag);

	UFUNCTION(BlueprintPure)
	TArray<UBaseItem*> GetAllItem() { return EquipArray; }

	/** Ищет эквип менеджер в объекте */
	UFUNCTION(BlueprintCallable, Category = "EquipManager|BaseEquipManager|Tools")
	static UBaseEquipManager* GetBaseEquipManager(AActor* FromActor);

	UFUNCTION(BlueprintCallable)
	void RecalculateBody();


protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	
	/*
	* Загрузка/Сохранение одежды
	*/
	virtual void Serialize(FArchive& Ar) override;
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
	void ClothUpdate(UBaseClothService* ClothUpdate);

	UFUNCTION(BlueprintNativeEvent)
	bool RemoveItem(class UBaseItem* Item);
	virtual bool RemoveItem_Implementation(class UBaseItem* Item) { return RemoveItemByItem(Item); }

	UFUNCTION(Server, Reliable)
	void Server_SetClothItem(UBaseItem* ClothItem);
	virtual void Server_SetClothItem_Implementation(UBaseItem* ClothItem) { SetClothItem(ClothItem); }
	
	UFUNCTION(Server, Reliable)
	void Server_RemoveItemByItem(UBaseItem* ClothItem);
	virtual void Server_RemoveItemByItem_Implementation(UBaseItem* ClothItem) { RemoveItemByItem(ClothItem); }
	

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(UCoreInventoryObject* Item);
	virtual bool I_CanRemoveItem_Implementation(UCoreInventoryObject* Item) { return true; }

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(UCoreInventoryObject* Item);
	virtual bool I_RemoveItem_Implementation(UCoreInventoryObject* Item) { return RemoveItemByItem(Cast<UBaseItem>(Item)); }

	UFUNCTION()
	void StartBuildBody();

	UFUNCTION()
	void OnRep_EquipArray();

	UFUNCTION()
	void OnRep_FullArmorData();

	UFUNCTION()
	void OnRep_BodyArmorData();
		
};
