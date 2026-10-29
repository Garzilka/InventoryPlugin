// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseHandItemService.h"
#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"
#include "InventoryEcosystem/HandItemSystem/Data/WeaponData.h"
#include "BaseWeaponService.generated.h"

USTRUCT()
struct FWeaponServiceSave
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TSubclassOf<UBaseItem> SaveItem;

	UPROPERTY(SaveGame)
	FName SocketUID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponServiceUpdateSignature, class UBaseWeaponService*, Ref);

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class HANDITEMSYSTEM_API UBaseWeaponService : public UBaseItemService, public IItemManagmentInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponServiceUpdateSignature OnWeaponServiceUpdate;

protected:
	UPROPERTY(SaveGame)
	TArray<FWeaponServiceSave> SaveItemData;

	UPROPERTY(Replicated, SaveGame)
	int32 CurrentAmountAmmo = 0;

	UPROPERTY(EditDefaultsOnly, Replicated)
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие")
	class UBaseWeaponData* WeaponData;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Настройки модулей"))
	TArray<FWeaponSocketInfo> ModificationData;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Заблоикрованные сокеты"))
	TArray<FName> SocketsBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Оружие")
	TMap<TSubclassOf<UBaseItem>, TSubclassOf<class ABaseBulit>> SupportedAmmo;

	UPROPERTY()
	TSubclassOf<class ABaseBulit> CurrentBulit = NULL;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Оружие", Replicated)
	TSubclassOf<UBaseItem> CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Оружие", meta = (DisplayName = "Урон от выстрела"))
	float DamagePerFire = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Анмиация взятия"))
	UAnimMontage* EquipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Оружие", meta = (DisplayName = "Анмиация убирания"))
	UAnimMontage* UnequipAnimation;
public:
	UBaseWeaponService();
	virtual void BeginPlay(UBaseItem* Item) override;

	UFUNCTION()
	void PlayEquipAnimation(UBaseHandItemService* CharacterInstigator, bool IsEquip);

	void SubstructAmmo(int32 Amount);

	virtual TArray<UObject*> GetAllChieldObject() override;
	virtual TArray<UBaseItem*> GetAttachedItem()  override;

	UFUNCTION(BlueprintPure)
	const int32& GetCurrentAmmo() const { return CurrentAmountAmmo; }

	UFUNCTION(BlueprintPure)
	const int32& GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentAmmo(int32 NewAmountAmmo) { CurrentAmountAmmo = NewAmountAmmo; }

	UFUNCTION(BlueprintCallable)
	bool AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate = true);

	UFUNCTION(BlueprintCallable)
	void AddBlockSocket(FName SocketUID);

	UFUNCTION(BlueprintCallable)
	void RemoveBlockSocket(FName SocketUID);

	UFUNCTION(BlueprintCallable)
	TMap<TSubclassOf<UBaseItem>, TSubclassOf<class ABaseBulit>> GetSupportedAmmo() const { return SupportedAmmo; };

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class ABaseBulit> GetCurrentBulit();

	UFUNCTION(BlueprintPure)
	TArray<UBaseWeaponModuleService*> GetAttachedModules(bool OnlySelf);

	UFUNCTION(BlueprintPure)
	UBaseItem* GetModificationBySocket(FWeaponSocketInfo SocketInfo);

	UFUNCTION(BlueprintPure)
	TArray<UBaseItem*> GetModulesByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	bool HasModuleByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetSpawnedRef();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAmmo(int32 Target);
	int32 GetAmmo_Implementation(int32 Target) { return INDEX_NONE; };

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAmountAmmo();
	int32 GetAmountAmmo_Implementation() { return INDEX_NONE; };

	UFUNCTION(BlueprintNativeEvent)
	bool ChangeMag(int32 InCurrentAmmo);
	bool ChangeMag_Implementation(int32 InCurrentAmmo) { return false; };

	UFUNCTION(BlueprintNativeEvent)
	bool CanChangeMag(int32 InCurrentAmmo);
	bool CanChangeMag_Implementation(int32 InCurrentAmmo) { return false; };

	UFUNCTION(BlueprintCallable)
	bool DetachItem(UBaseItem* Item);

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(UCoreInventoryObject* Item);
	virtual bool I_RemoveItem_Implementation(UCoreInventoryObject* Item) { return DetachItem(Cast<UBaseItem>(Item)); }

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(UCoreInventoryObject* Item);
	virtual bool I_CanRemoveItem_Implementation(UCoreInventoryObject* Item) { return true; }

	UFUNCTION()
	void Event_PreSpawnItem(UBaseWorldItemService* WorldService, ABaseItemActor* SpawnedReference);
	 virtual void Serialize(FArchive& Ar) override;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Event_WeaponFire(class ABaseFireWeapon* Weapon);

	UFUNCTION()
	void Event_UpdateModule(class UBaseWeaponModuleService* ModuleRef);

	UFUNCTION()
	void OnRep_AttachedModules();

	UFUNCTION(Server, Reliable)
	void Server_AttachItem(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate);
	void Server_AttachItem_Implementation(UBaseItem* Item, FWeaponSocketInfo TargetPlacement, bool NeedCreate);
};
