// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseSpatialInventory.h"
#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"
#include "BaseInventoryManager.generated.h"


USTRUCT(BlueprintType)
struct FClothInventoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Предмет владелец"))
	UBaseItem* ItemOwner;

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Инвентарь"))
	UBaseLineInventory* Inventory;
};


// #define LOG_INVENTORYMANAGER(type, text) UInventoryLOG::PrintInventoryLog(GetOwner(), type, text, __LINE__, __FUNCTION__)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateInventoryesSignature, class UBaseInventoryManager*, ManagerRef);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYMANAGERSYSTEM_API UBaseInventoryManager : public UInventoryCoreComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateInventoryesSignature OnUpdateInventoryes;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateInventoryesSignature OnInventoryManagerUpdate;

protected:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Inventory)
	TArray<FClothInventoryData> Inventoryes;
	
public:	
	UBaseInventoryManager();
	const TArray<FClothInventoryData>& GetInventoryData();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UBaseInventoryManager* GetBaseInventoryManager(AActor* From);
	
	UFUNCTION(BlueprintCallable)
	bool AddInventory(UBaseItem* Ref);

	UFUNCTION(BlueprintCallable)
	bool RemoveInventory(UBaseItem* Ref);

	UFUNCTION(BlueprintCallable)
	bool CanAddItem(UBaseItem* Ref);

	UFUNCTION(BlueprintCallable)
	UBaseLineInventory* FindInventory(UBaseItem* Ref);

	UFUNCTION(BlueprintCallable)
	int32 GetAllCountByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass);

	UFUNCTION(BlueprintCallable)
	int32 SubstructItem(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass);

	UFUNCTION(BlueprintCallable)
	TArray<UBaseItem*> GetAllItemByClass(TSubclassOf<UBaseItem> ItemClass, bool ReverseArray, bool IsCheckOnParrentClass);

	/*
	* Вернет все предметы по классу
	* @ItemClassArray - Массив классов
	* @OnlyOneByClass - Если имеется несколько предметов по данному классу, вернет только один из них (если параметр true)
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UBaseItem*> GetAllItemByClassArray(TArray<TSubclassOf<UBaseItem>> ItemClass, bool OnlyOneByClass);

	/*
	* Вернет все предметы по типу
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UBaseItem*> GetAllItemByType(FGameplayTag ItemType);

	UFUNCTION(BlueprintCallable)
	bool AddTryItem(UBaseItem* NewItem, bool IsNeedStackItem, bool WithDelegate = true);

	UFUNCTION(BlueprintCallable)
	TArray<UBaseItem*> GetAllItem();
protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_AddInventory(UBaseItem* Ref);
	void Server_AddInventory_Implementation(UBaseItem* Ref);

	UFUNCTION(Server, Reliable)
	void Server_RemoveInventory(UBaseItem* Ref);
	void Server_RemoveInventory_Implementation(UBaseItem* Ref);

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION()
	void UpdateInventory(class UBaseLineInventory* InventoryRef);
	/*
	* Загрузка/Сохранение инвентаря
	*/
	virtual void Serialize(FArchive& Ar) override;
		
};
