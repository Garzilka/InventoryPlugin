// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseSpatialInventory.h"
#include "BaseInventoryItemService.generated.h"


USTRUCT()
struct FInventoryManagerSlotSave
{
	GENERATED_BODY()

	bool IsSaved = false;
	int32 NumArray = 0;
	TArray<uint8> InventoryData = {};

	FInventoryManagerSlotSave() {};
	FInventoryManagerSlotSave(class UBaseInventoryItemService* Data);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FInventoryManagerSlotSave& Value)
	{
		if (Ar.IsSaving())
		{
			Value.NumArray = Value.InventoryData.Num();
			Ar << Value.IsSaved;
			Ar << Value.NumArray;
			Ar << Value.InventoryData;
		}

		if (Ar.IsLoading())
		{
			Ar << Value.IsSaved;
			Ar << Value.NumArray;
			Value.InventoryData.SetNum(Value.NumArray);
			Ar << Value.InventoryData;
		}
		return Ar;
	}

};


UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class INVENTORYMANAGERSYSTEM_API UBaseInventoryItemService : public UBaseItemService
{
	GENERATED_BODY()
	
protected:
	
	FInventoryManagerSlotSave SavedData;
	
	UPROPERTY(BlueprintReadWrite, Category = "Инвентарь", EditAnywhere, meta = (DisplayName = "Размер инвентаря"))
	FIntPoint SizeInventory = FIntPoint::ZeroValue;
	
	UPROPERTY(BlueprintReadWrite, Category = "Инвентарь", EditAnywhere, meta = (DisplayName = "Размер инвентаря"))
	TSubclassOf<UBaseLineInventory> InventoryClass;

	UPROPERTY(BlueprintReadOnly, Category = "Инвентарь")
	UBaseLineInventory* InventoryRef = nullptr;
public:
	virtual void BeginPlay(UBaseItem* Item) override; 
	 virtual void Serialize(FArchive& Ar) override;
	
	UFUNCTION(BlueprintPure)
	FIntPoint GetSizeInventory() const {	return SizeInventory; }

	UFUNCTION(BlueprintPure)
	UBaseLineInventory* GetInventoryRef() const {	return InventoryRef; }

	UFUNCTION(BlueprintCallable)
	UBaseLineInventory* MakeInventory(AActor* InventoryOwner = nullptr);

	UFUNCTION(BlueprintCallable)
	void SafeInventoryState();

	UFUNCTION(BlueprintCallable)
	void DestroyAndSafeInventory();

protected:
	UFUNCTION()
	void Event_ItemEquiped(UBaseItem* ItemRef);
	
	UFUNCTION()
	void Event_ItemUnequiped(UBaseItem* ItemRef);
};
