// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"
#include "InventoryEcosystem/InventoryCore/Interface/ItemManagmentInterface.h"

#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"

#include "BaseLineInventory.generated.h"



USTRUCT()
struct FInventoryItemSave
{
	GENERATED_BODY()


	TSubclassOf<UBaseItem> SavedClass;

	int32 NumArray = 0;
	TArray<uint8> ItemData;

public:
	FInventoryItemSave() {};
	FInventoryItemSave(UBaseItem* Item);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FInventoryItemSave& Value)
	{
		if (Ar.IsSaving())
		{
			Ar << Value.SavedClass;
			Value.NumArray = Value.ItemData.Num();
			Ar << Value.NumArray;
			Ar << Value.ItemData;
		}

		if (Ar.IsLoading())
		{
			Ar << Value.SavedClass;
			Ar << Value.NumArray;
			Value.ItemData.SetNum(Value.NumArray);
			Ar << Value.ItemData;
		}
		return Ar;
	}
};

USTRUCT()
struct FInventorySave
{
    GENERATED_BODY()

    bool Saved = false;
    int32 NumArray = 0;
    FIntPoint Size = FIntPoint::ZeroValue;
    TArray<FInventoryItemSave> SavedData;

    bool IsValidData() { return Saved; };

public:
    FInventorySave() {};
    FInventorySave(TMap<UBaseItem*, FIntPoint> Data, AActor* Owner);
    FInventorySave(TMap<UBaseItem*, FIntPoint> Data, int32 ColumSize, AActor* Owner);



    FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FInventorySave& Value)
    {
        if (Ar.IsSaving())
        {
            Ar << Value.Saved;
            Ar << Value.Size;
            Value.NumArray = Value.SavedData.Num();
            Ar << Value.NumArray;
            Ar << Value.SavedData;
        }

        if (Ar.IsLoading())
        {
            Ar << Value.Saved;
            Ar << Value.Size;
            Ar << Value.NumArray;
            Value.SavedData.SetNum(Value.NumArray);
            Ar << Value.SavedData;
        }
        return Ar;
    }
};



USTRUCT()
struct FNetInventoryData
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FIntPoint InventorySize;

public:
	UPROPERTY()
	TArray<UBaseItem*> Inventory;

	void SetSize(FIntPoint NewSize)
	{
		InventorySize.X = (NewSize.X > 0 ? NewSize.X : 1);
		InventorySize.Y = (NewSize.Y > 0 ? NewSize.Y : 1);
	}

	int32 GetNumBySize()
	{
		return (InventorySize.X * InventorySize.Y);
	}

	FIntPoint GetSize() { return InventorySize; }
};

USTRUCT()
struct FInventoryCachedData
{
	GENERATED_BODY()

public:
	bool IsValidCache = false;
	bool IsValidGridCache = false;

	void InvalidateCache() { Cache.Reset(); GridCache.Reset();  IsValidCache = false; IsValidGridCache = false; }


	UPROPERTY()
	TArray<UBaseItem*> Cache;

	UPROPERTY()
	TMap<UBaseItem*, FIntPoint> GridCache;
};

/** Инвентарь обновился */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateInventorySignature, class UBaseLineInventory*, InventoryRef);

/*
 * Вес инвентаря изменился.
 * @NewWeight - Новый вес
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeightChangeSignature, float, NewWeight);


/*
 * Предмет покинул инвентарь или скрыт в инвентаре.
 * Вызов - две стороны
 * ItemRef - Предмет
 * IsLeaveInventory - Покинул инвентарь
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOutItemSignature, class UBaseLineInventory*, InventoryRef, UBaseItem*, ItemRef, bool, IsLeaveInventory);

/*
 * Новый предмет инвентаря.
 * Вызов - две стороны
 * ItemRef - новый предмет
 * IsNewItem - Если false оперирование внутри инвентаря и необходимо его отрисовать, если true это новый предмет, которого раньше не было
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNewItemSignature, class UBaseItem*, ItemRef, FIntPoint, Position, bool, IsNewItem);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UBaseLineInventory : public UInventoryCoreComponent, public IItemManagmentInterface
{
	GENERATED_BODY()

public:
	/** ��������� ����� ��������� ������, �������� ��������� ������ ��������� */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateInventorySignature OnUpdateInventory;

	/** ��������� ����� ��������� ������ ��� ����� ������� ����������� ������ ��������� */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FUpdateInventorySignature OnUpdateInventoryItems;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FNewItemSignature OnNewItem;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOutItemSignature OnLeaveItem;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeightChangeSignature OnWeightChange;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "���������|��������", meta = (AllowPrivateAccess = "true", DisplayName = "������ ���������", DisplayPriority = -1))
	FIntPoint DefaultInventorySize;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	FNetInventoryData InventoryData;

	UPROPERTY(ReplicatedUsing = OnRep_InventoryWeight)
	float InventoryWeight = 0.f;

	UPROPERTY()
	FInventoryCachedData ItemCache;

	UPROPERTY()
	bool AsGrid = false;

	/** ������ ���������� ���� ��������� */
	FTimerHandle RecalculateWeight;

	/** ������ ���������� ��������� */
	FTimerHandle OnRepRemaining;

	UPROPERTY()
	FInventorySave SavedData;
public:	
	UBaseLineInventory();
	FInventorySave GetSaveData();
	void LoadSaveData(FInventorySave Save);
	/**/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Tools")
	void ClearInventory();

	/** ���� �������� ��������� � ������� */
	UFUNCTION(BlueprintCallable, Category = "UnrealInventorySystem|Inventory|Tools")
	static UBaseLineInventory* GetLineInventory(AActor* FromActor);

	UFUNCTION(BlueprintPure)
	FIntPoint GetSize() { return InventoryData.GetSize(); };

	/*
	* ����������� ������� � ������ �� ���������� ���������
	*/
	UFUNCTION(BlueprintPure, Category = "UnrealInventorySystem|Inventory|Tools")
	virtual int32 ConvertPositionToIndex(FIntPoint InTitle) { return InTitle.X; }

	/*
	* ����������� ������� � ������� �� ���������� ���������
	*/
	UFUNCTION(BlueprintPure, Category = "UnrealInventorySystem|Inventory|Tools")
	virtual FIntPoint ConvertIndexToPosition(int32 Index) { return FIntPoint(Index, 0); };

	/*
	* ��������� ���������� �������
	*/
	UFUNCTION(BlueprintPure, Category = "InventorySystem|Inventory|Tools")
	virtual bool IsPositionValid(FIntPoint InTitle) { return (InTitle.Y >= 0 && InTitle.Y < InventoryData.Inventory.Num()); }

	/*
	* ��������� ���������� �������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Tools")
	virtual bool Contains(UBaseItem* ItemRef);

	/*
	* �������� ������� �� ����������� ��������� ��������, �������� ������ �������� � ����������� ����������� ��������.
	* ���������� true ���� �������.
	* ������ false ���� ����� ������ ��������� ������ ��� ������������ ������ ��� �������� ������ ����������
	* @ItemRef - ������� �� ��������
	* @Index - ������� ��������
	* @IsNeedStackItems - ��������� ����������� �����������?
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Tools")
	virtual bool IsPositionAvailable(UBaseItem* ItemRef, int32 Index, bool IsNeedStackItems);

	/*
	* ���������� ������ ��������� � �� ��������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	TArray<UBaseItem*> GetAllItem();

	/*
	* ������ ��� �������� �� ������
	* @ItemClass - ����� ��������
	* @ReverseArray - ������ ����� ����� �� ������ ��������� ��� �� ����� ���������
	* @IsCheckOnParrentClass - �������� �� �������� ������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	TArray<UBaseItem*> GetAllItemByClass(TSubclassOf<UBaseItem> ItemClass, bool ReverseArray = false, bool IsCheckOnParrentClass = false);

	/*
	* ������ ��� �������� �� ������
	* @ItemClassArray - ������ �������
	* @OnlyOneByClass - ���� ������� ��������� ��������� �� ������� ������, ������ ������ ���� �� ��� (���� �������� true)
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	TArray<UBaseItem*> GetAllItemByClassArray(TArray<TSubclassOf<UBaseItem>> ItemClassArray, bool OnlyOneByClass);

	/*
	* ������ ��� �������� �� ������
	* @ItemType - ������ �������
	* @OnlyOneByClass - ���� ������� ��������� ��������� �� ������� ������, ������ ������ ���� �� ��� (���� �������� true)
	*/
	UFUNCTION(BlueprintCallable)
	TArray<UBaseItem*> GetAllItemByType(FGameplayTag ItemType);

	/*
	* ���������� ������ ��������� � �� ��������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	TMap<UBaseItem*, FIntPoint> GetAllItemGrid();

	/*
	* ������ ��� �������� �� ������
	* @ItemClass - ����� ��������
	* @IsCheckOnParrentClass - �������� �� �������� ������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	int32 GetAllCountByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass = false);

	/*
	* ������ ����������, ������� ���������� �������
	* @ItemClass - ����� ��������
	* @TargetAmount - ������� ����������
	* @IsCheckOnParrentClass - �������� �� �������� ������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	int32 SubstructItem(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass = false);

	/*
	* ������ ������ ���������� � ��������� ������� �� ������
	* @ItemClass - ����� ��������
	* @IsCheckOnParrentClass - �������� �� �������� ������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	UBaseItem* GetFirstItemByClass(TSubclassOf<UBaseItem> ItemClass, bool IsCheckOnParrentClass = false);

	/*
	* ������ ������� �� �������. ���� ������ �� ������� ��� �������� ���, �������� ������� (nullptr)
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Get")
	FORCEINLINE UBaseItem* GetItemAtIndex(int32 Idx) { return (InventoryData.Inventory.IsValidIndex(Idx) ? InventoryData.Inventory[Idx] : nullptr); };

	/*
	* ��������� �������������� �� ����� ������������ ������������, �������,
	* ���� ��������� �����������, ��� �������� ������� �� ����������� ������ ��������� InventoryManager
	* @Result - ������ ��������, ������� �� �����������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Size")
	virtual bool SetSize(FIntPoint NewSize, TArray<UBaseItem*>& Result);

	/*
	* �������� ������� � ������ ��������� �������. ���������� true ���� �������
	* @NewItem - ������� �� ����������
	* @IsNeedStackItem - ����� ����������?
	* @WithDelegate - ����� ������� OnNewItem?
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Add")
	virtual bool AddTryItem(UBaseItem* NewItem, bool IsNeedStackItem, bool WithDelegate = true);

	/*
	* �������� ������� � ��������� �������. ���������� true ���� �������
	* @NewItem - ������� �� ���������
	* @TopLeftIndex - ������� ��� ���������
	* @IsNeedStackItem - ����� ����������?
	* @WithDelegate - ����� ������� OnNewItem?
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Add")
	virtual bool AddItemAt(UBaseItem* NewItem, int32 TopLeftIndex, bool IsNeedStackItem);

	/*
	* ������� ������� �� ������ �� �������. ���������� true ���� �������.
	* @ItemRef - ������ �� �������
	* @WithDelegate - ����� �������� ������� OnItemLeaveFromGrid?
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Remove")
	virtual bool RemoveItemByItem(UBaseItem* ItemRef, bool WithDelegate = true);

	/*
	* ������� ������� �� ��������� �������. ���������� true ���� �������.
	* @TopLeftIndex - ������� �� ��������
	* @WithDelegate - ����� �������� ������� OnItemLeaveFromGrid?
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Remove")
	virtual bool RemoveItemAt(int32 TopLeftIndex, bool WithDelegate = true);

	/*
	* ����������� ������� �� ����� � �����. ���������� true ���� �������.
	* @TopLeftIndex - ������� �� ��������
	* @TopLeftIndexTo - ������� �� ���������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Move")
	virtual bool MoveItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);

	/*
	* �������� �������� �������.
	* @TopLeftIndex - ������� ��������
	* @TopLeftIndexTo - ������� ��������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Move")
	virtual bool SwapItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);

	/*
	* �������� ����������� �������� ����������� ���������.
	* @TopLeftIndex - ������� ��������
	* @TopLeftIndexTo - ������� ��������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Move")
	virtual bool CanSwap(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);

	/*
	* �������� ����������� �������� ����������� ���������.
	* @TopLeftIndex - ������� ��������
	* @TopLeftIndexTo - ������� ��������
	*/
	UFUNCTION(BlueprintCallable, Category = "InventorySystem|Inventory|Move")
	virtual bool CanAddItem(UBaseItem* Item, int32 Amount = -1, bool WithStack = true);
	
	/*
	* Загрузка/Сохранение инвентаря
	*/
	virtual void Serialize(FArchive& Ar) override;
protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintNativeEvent)
	bool I_RemoveItem(class UCoreInventoryObject* Item);
	virtual bool I_RemoveItem_Implementation(class UCoreInventoryObject* Item) { return RemoveItemByItem(Cast<UBaseItem>(Item)); }

	UFUNCTION(BlueprintNativeEvent)
	bool I_CanRemoveItem(class UCoreInventoryObject* Item);
	virtual bool I_CanRemoveItem_Implementation(class UCoreInventoryObject* Item) { return true; }

	UFUNCTION(Reliable, Server)
	void Server_AddTryItem(UBaseItem* InItem, bool IsNeedStackItem, bool WithDelegate);
	void Server_AddTryItem_Implementation(UBaseItem* InItem, bool IsNeedStackItem, bool WithDelegate);

	UFUNCTION(Reliable, Server)
	void Server_AddItemAt(UBaseItem* InItem, int32 TopLeftIndex, bool IsNeedStackItem);
	void Server_AddItemAt_Implementation(UBaseItem* InItem, int32 TopLeftIndex, bool IsNeedStackItem);

	UFUNCTION(Reliable, Server)
	void Server_RemoveItemAt(int32 TopLeftIndex, bool WithDelegate = true);
	void Server_RemoveItemAt_Implementation(int32 TopLeftIndex, bool WithDelegate = true);

	UFUNCTION(Reliable, Server)
	void Server_SwapItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);
	void Server_SwapItem_Implementation(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);

	UFUNCTION(Reliable, Server)
	void Server_MoveItem(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);
	void Server_MoveItem_Implementation(int32 TopLeftIndexFrom, int32 TopLeftIndexTo);

	UFUNCTION(Reliable, Server)
	void Server_SubstructItem(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass = false);
	void Server_SubstructItem_Implementation(TSubclassOf<UBaseItem> ItemClass, int32 TargetAmount, bool IsCheckOnParrentClass = false);

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
	/*
	* Сохранение инвентаря
	*/
	virtual void SaveData();

	/*
	* �������� �� ���������� ��������
	*/
	UFUNCTION()
	void BindItem(UBaseItem* ItemRef);

	/*
	* ������� �� ���������� ��������
	*/
	UFUNCTION()
	void UnbindItem(UBaseItem* ItemRef);

	/*
	* ���������� ��������
	*/
	UFUNCTION()
	void ItemUpdate(UBaseItem* ItemRef);

	/*
	* ���������� �������� ���� ���������
	*/
	void CalculateCurrentWeight();
	UFUNCTION()
	void UpdateWeight();

	/*
	* ��������� ���������
	*/
	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION()
	virtual void InventoryUpdate();

	/*
	* ��������� ���������
	*/
	UFUNCTION()
	virtual void OnRep_InventoryWeight();
		
};
