// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseLineInventory.h"
#include "BaseGridInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UBaseGridInventory : public UBaseLineInventory
{
	GENERATED_BODY()

public:	
	UBaseGridInventory();
	
	/** Ищет гридовый инвентарь в объекте */
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Tools")
	static UBaseGridInventory* GetGridInventory(AActor* FromActor);

	/** Возвращает количество колонок в гриде инвентаря */
	UFUNCTION(BlueprintPure, Category = "InventorySystem|Inventory|Tools")
	int32 GetRows() {return InventoryData.GetSize().Y; }

	/** Возвращает количество колонок в гриде инвентаря */
	UFUNCTION(BlueprintPure, Category = "InventorySystem|Inventory|Tools")
	int32 GetColums() {return InventoryData.GetSize().X;}

	/** Конвертация индекса в позицию по настройкам инвентаря */
	virtual FIntPoint ConvertIndexToPosition(int32 Index) override { return FIntPoint(Index % GetColums(), Index / GetColums()); }

	/** Конвертация позиции в индекс по настройкам инвентаря */
	virtual int32 ConvertPositionToIndex(FIntPoint InTitle) override { return InTitle.X + (InTitle.Y * GetColums()); }

protected:
	virtual void BeginPlay() override;

	/** Проверяет валидность позиции */
	virtual bool IsPositionValid(FIntPoint InTitle) override { return (InTitle.X >= 0 && InTitle.Y >= 0 && InTitle.X < GetColums() && InTitle.Y < GetRows()); }

	virtual void InventoryUpdate() override;

	FTimerHandle OnRepRemaining;
	UFUNCTION()
	void ReCalculateInventory();

	virtual void SaveData() override;
		
};
