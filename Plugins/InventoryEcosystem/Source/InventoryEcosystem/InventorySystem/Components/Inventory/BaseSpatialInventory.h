// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGridInventory.h"
#include "BaseSpatialInventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UBaseSpatialInventory : public UBaseGridInventory
{
	GENERATED_BODY()

public:	
	UBaseSpatialInventory();

	virtual bool IsPositionAvailable(UBaseItem* ItemRef, int32 Index, bool IsNeedStackItems) override;


	/*
	* Добавить предмет в указанную позицию учитывая размер предмета. Возвращает true если удалось
	* @ItemRef - предмет на установку
	* @TopLeftIndex - Позиция для установки
	* @IsNeedStackItem - Нужно объединить?
	* @WithDelegate - Нужен делегат OnNewItem?
	*/
	virtual bool AddItemAt(UBaseItem* ItemRef, int32 TopLeftIndex, bool IsNeedStackItem) override;

	/*
	* Удалить предмет по указанной позиции. Возвращает true если удалось.
	* @TopLeftIndex - Позиция на удаление
	* @WithDelegate - Нужно вызввать делегат OnItemLeaveFromGrid?
	*/
	virtual bool RemoveItemAt(int32 TopLeftIndex, bool WithDelegate = true) override;

	virtual bool CanAddItem(UBaseItem* Item, int32 Amount = -1, bool WithStack = true) override;
	
protected:
	virtual void BeginPlay() override;
		
};
