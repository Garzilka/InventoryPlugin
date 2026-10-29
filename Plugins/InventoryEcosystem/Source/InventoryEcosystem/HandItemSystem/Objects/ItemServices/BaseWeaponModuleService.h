// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHandItemService.h"
#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/HandItemSystem/Data/WeaponData.h"
#include "UObject/NoExportTypes.h"
#include "BaseWeaponModuleService.generated.h"

USTRUCT()
struct FWeaponModuleServiceSave
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TSubclassOf<UBaseItem> SaveItem;

	UPROPERTY(SaveGame)
	FName SaveTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponModuleServiceUpdateSignature, class UBaseWeaponModuleService*, Ref);

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class HANDITEMSYSTEM_API UBaseWeaponModuleService : public UBaseItemService, public IItemManagmentInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponModuleServiceUpdateSignature OnWeaponModuleServiceUpdate;


protected:
	UPROPERTY(SaveGame)
	TArray<FWeaponModuleServiceSave> SaveItemData;

	UPROPERTY(Replicated, SaveGame)
	int32 CurrentAmountAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Максимальное количество патрон"))
	int32 MaxAmmo = 30;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Оружие", Replicated)
	TSubclassOf<UBaseItem> AmmoClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Тег модификации"))
	FGameplayTag ModificationTag;

	UPROPERTY(EditDefaultsOnly, Category = "Оружие")
	FGameplayTagContainer SupportedModifiaction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Настройки модуля"))
	TMap<TSubclassOf<UBaseItem>, FWeaponModuleInfo> ModItemInfo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (ForceInlineRow, DisplayName = "Настройки модулей"))
	TArray<FWeaponSocketInfo> ModificationData;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (ForceInlineRow, DisplayName = "Блокировать слот"))
	TArray<FName> BlockSlotBySocketUID;

	UPROPERTY()
	FGameplayTagContainer CurrentSupportedModifiaction;
public:
	UPROPERTY(Replicated)
	FWeaponSocketInfo InstalTo;

	UPROPERTY(Replicated)
	class UBaseItemService* InstalPlace = nullptr;

	UPROPERTY(Replicated, DuplicateTransient)
	class UBaseWeaponService* WeaponOwner = nullptr;

	UFUNCTION(BlueprintPure)
	const int32& GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintPure)
	const TArray<FName>& GetBlockSlotBySocketUID() const { return BlockSlotBySocketUID; }
	 
	UFUNCTION(BlueprintPure)
	const TArray<FWeaponSocketInfo>& GetModificationData() const { return ModificationData; }

	UFUNCTION(BlueprintPure)
	const int32& GetCurrentAmmo() const { return CurrentAmountAmmo; }

	UFUNCTION(BlueprintCallable)
	void SubstructAmmo(int32 Value) { CurrentAmountAmmo -= Value; }

	UFUNCTION(BlueprintPure)
	const FGameplayTag& GetModificationTag() { return ModificationTag; }

	UFUNCTION(BlueprintPure)
	bool GetModificationInfoItem(TSubclassOf<UBaseItem> ByWeaponItem, FWeaponModuleInfo& Info);

	UFUNCTION(BlueprintPure)
	TArray<UBaseItem*> GetModificationsByTag(FGameplayTag Tag);
	
	UFUNCTION(BlueprintPure)
	UBaseItem* GetModificationBySocket(FWeaponSocketInfo SocketInfo);
	
	UFUNCTION(BlueprintCallable)
	bool AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate = true);

	UFUNCTION(BlueprintCallable)
	bool DetachItem(UBaseItem* Item);

	UFUNCTION(BlueprintCallable)
	void DetachBySocketUID(FName SocketUID);

	UFUNCTION(BlueprintCallable)
	void DetachAllAttaches();

	UFUNCTION(BlueprintPure)
	TArray<UBaseWeaponModuleService*> GetAttachedModules(bool OnlySelf);

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(class UCoreInventoryObject* Item);
	virtual bool I_RemoveItem_Implementation(class UCoreInventoryObject* Item) { return DetachItem(Cast<UBaseItem>(Item)); }

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(class UCoreInventoryObject* Item);
	virtual bool I_CanRemoveItem_Implementation(class UCoreInventoryObject* Item) { return true; }

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetCurrentSupportedModifiaction() { return SupportedModifiaction ; }

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAmmo(int32 Target);
	int32 GetAmmo_Implementation(int32 Target) { return INDEX_NONE; };

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAmountAmmo();
	int32 GetAmountAmmo_Implementation() { return INDEX_NONE; };

	void RefreshAttachment();

	virtual void Serialize(FArchive& Ar) override;

public:
	// virtual TArray<UObject*> GetAllChieldObject() override;
	// virtual TArray<UBaseItem*> GetAttachedItem()  override;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Event_UpdateModule(UBaseWeaponModuleService* ModuleRef);

	UFUNCTION(Server, Reliable)
	void Server_AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate);
	void Server_AttachItem_Implementation(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate);

	UFUNCTION()
	void OnRep_AttachedModules();
	
};
