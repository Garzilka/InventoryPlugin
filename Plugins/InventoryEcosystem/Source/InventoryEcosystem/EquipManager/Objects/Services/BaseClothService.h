// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BaseAttachClothService.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h"
#include "InventoryEcosystem/EquipManager/Data/CharacterMeshData.h"
#include "BaseClothService.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClothItemUpdateSignature, class UBaseClothService*, ClothUpdate);

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class EQUIPMANAGER_API UBaseClothService : public UBaseItemService
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FClothItemUpdateSignature OnClothUpdate;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Одежда", meta = (DisplayName = "Тип тела"))
	FGameplayTag BodyTag;

	UPROPERTY(EditDefaultsOnly, Category = "Одежда", meta = (DisplayName = "Слоты одежды"))
	FGameplayTagContainer ClothType;

	UPROPERTY(EditDefaultsOnly, Category = "Одежда", meta = (DisplayName = "Скрытие базового меша"))
	FGameplayTagContainer HideBody;

	UPROPERTY(EditDefaultsOnly, Category = "Слоты экипировки", meta = (DisplayName = "Открыть слоты для экипировки"))
	FGameplayTagContainer OpenEquipSlots;

	UPROPERTY(EditDefaultsOnly, Category = "Слоты экипировки", meta = (DisplayName = "Закрыть слоты для экипировки"))
	FGameplayTagContainer CloseEquipSlots;

	UPROPERTY(BlueprintReadWrite, Category = "Одежда", EditAnywhere)
	TMap<FName, TSoftObjectPtr<USkeletalMesh>> GenderData;

	UPROPERTY(BlueprintReadWrite, Category = "Одежда", EditAnywhere)
	TMap<FName, FClothData> ClothGenderData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer BaseSupportModification;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTagContainer SupportedModification;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attaches)
	TArray<UBaseAttachClothService*> Attaches;
public:
	virtual void BeginPlay(UBaseItem* Item) override;

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetClothType() { return ClothType; };

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetOpenEquipSlots();

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetCloseEquipSlots();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetBodyTag() { return BodyTag; };

	UFUNCTION(BlueprintPure)
	bool HasClothType(FGameplayTag InType);

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetHideBodyMask() {return HideBody;}

	UFUNCTION(BlueprintPure)
	FClothData GetMeshData(FName Gender);

	UFUNCTION()
	void Event_PreSpawnItem(UBaseWorldItemService* WorldService, ABaseItemActor* SpawnedReference);

	UFUNCTION(BlueprintCallable)
	bool AttachItem(UBaseItem* Item, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	bool DetachItem(UBaseItem* Item);

	 bool DetachClothService(UBaseAttachClothService* Item);

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(class UBaseItem* Item);
	virtual bool I_RemoveItem_Implementation(class UBaseItem* Item) { return DetachItem(Item); }

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(class UBaseItem* Item);
	virtual bool I_CanRemoveItem_Implementation(class UBaseItem* Item) { return true; }

	UFUNCTION(BlueprintPure)
	UBaseAttachClothService* GetAttachByAttachSlot(FGameplayTag TagAttach);

	UFUNCTION(BlueprintPure)
	TArray<UBaseAttachClothService*> GetAttaches() { return Attaches; }

	virtual TArray<UObject*> GetAllChieldObject() override;
	virtual TArray<UBaseItem*> GetAttachedItem()  override;

	virtual void Serialize(FArchive& Ar) override;

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Attaches();

	UFUNCTION(Server, Reliable)
	void Server_AttachItem(UBaseItem* Item, FGameplayTag Tag);
	void Server_AttachItem_Implementation(UBaseItem* Item, FGameplayTag Tag);

	UFUNCTION(Server, Reliable)
	void Server_DetachItem(UBaseItem* Item);
	void Server_DetachItem_Implementation(UBaseItem* Item);
};
