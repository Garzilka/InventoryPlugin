// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystem/ItemSystem/Objects/MakeItemRule/MakeItemRule.h"
#include "InventoryLibrary.generated.h"


USTRUCT(BlueprintType)
struct FInventoryItemData 
{
	GENERATED_BODY()
		
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Класс предмета"))
	TSubclassOf<UBaseItem> ItemClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!bIsRange", EditConditionHides, DisplayPriority = 0, DisplayName = "Количество по умолчанию"))
	int32 Amount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsRange", EditConditionHides, DisplayPriority = 1, DisplayName = "Рандомное количество"))
	FIntPoint Range = FIntPoint(-1);


	bool IsValid() const
	{
		return UKismetSystemLibrary::IsValidClass(ItemClass.Get());
	}

	void DoRand()
	{
		if (!bIsRange) return;
		
		Amount = (Range == FIntPoint(-1) ? Amount : FMath::RandRange(Range.GetMin(), Range.GetMax()));
		Range = FIntPoint(-1);
	}
	FInventoryItemData() {};
	FInventoryItemData(TSubclassOf<UBaseItem> InItemClass) 
	{
		ItemClass = InItemClass;
		Amount = 1;
	};

	friend bool operator== (const FInventoryItemData& Left, const FInventoryItemData& Right)
	{
		return (Left.ItemClass == Right.ItemClass);
	}
};

USTRUCT(BlueprintType)
struct FMakeItemContext
{
    GENERATED_BODY()
		
public:
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCustom	= false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bIsCustom", EditConditionHides, DisplayName = "Настройки предмета"))
    FInventoryItemData ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsCustom", EditConditionHides))
    TSubclassOf<UMakeItemRule> MakeItemRule;
	
    bool IsValid() const
    {
        // if (bIsCustom)
        // {
        // 	return (MakeItemRule ? UKismetSystemLibrary::IsValidClass(MakeItemRule->GetItemClass()) : false);
        // }
        return ItemData.IsValid();
    }
    TSubclassOf<UBaseItem> GetItemClass()
    {
        // if (bIsCustom)
        // {
        // 	return (MakeItemRule ? MakeItemRule->GetItemClass() : TSubclassOf<UBaseItem>());
        // }
        return ItemData.ItemClass;
    }
    FMakeItemContext() {};
	
    friend bool operator== (const FMakeItemContext& Left, const FMakeItemContext& Right)
    {
        if (Left.bIsCustom || Right.bIsCustom)
        {
            return Left.MakeItemRule == Right.MakeItemRule;
        }
        return (Left.ItemData.ItemClass == Right.ItemData.ItemClass);
    }
};








USTRUCT(BlueprintType)
struct FSpawnItemContext
{
    GENERATED_BODY()

protected:
    UPROPERTY(Transient)
    UBaseItem* Item;
	
    UPROPERTY(Transient)
    FInventoryItemData ItemData;

public:

    bool WithSwitchParrent = true;

    bool bWithSpawnFinish = true;

    UPROPERTY(BlueprintReadWrite)
    bool bReplicateOnlyOwner = false;
	
    UPROPERTY(BlueprintReadWrite)
    FTransform SpawnLocation = {};

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<class ABaseItemActor> ClassToSpawn;

    UPROPERTY(BlueprintReadWrite)
    AActor* NewOwner = nullptr;

    FSpawnItemContext() {};


    FSpawnItemContext(UBaseItem* InItem, AActor* InNewOwner, FTransform InStartPosition, TSubclassOf<class ABaseItemActor> InClassToSpawn = {})
    {
        Item = InItem;
        NewOwner = InNewOwner;
        SpawnLocation = InStartPosition;
        ClassToSpawn = InClassToSpawn;
    }
	
    FSpawnItemContext(FInventoryItemData InItemData, AActor* InNewOwner, FTransform InStartPosition, TSubclassOf<class ABaseItemActor> InClassToSpawn = {})
    {
        ItemData = InItemData;
        NewOwner = InNewOwner;
        SpawnLocation = InStartPosition;
        ClassToSpawn = InClassToSpawn;
    }

    bool IsDataValid()
    {
        return (ItemData.IsValid() || Item != nullptr &&!SpawnLocation.GetLocation().IsNearlyZero());
    }

    UBaseItem* GetItem() const;
};

class UBaseItem;
UCLASS()
class ITEMSYSTEM_API UInventoryLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:	
    static UBaseItem* MakeItem(AActor* Owner, FMakeItemContext Item);
	
    static UBaseItem* MakeItem(AActor* Owner, FInventoryItemData Item);
	
    static TArray<UBaseItem*> MakeItems(AActor* Owner, FInventoryItemData Item);
	
    static TArray<UBaseItem*> MakeItems(AActor* Owner, FMakeItemContext Item);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    static class ABaseItemActor* SpawnItemObjectContext(const UObject* WorldContextObject, FSpawnItemContext InSpawnItemContext);
	
    UFUNCTION(BlueprintCallable, Category = "UnrealInventorySystem|InventoryUtilits")
    static float GetItemCellSize();
	
    UFUNCTION(BlueprintCallable, Category = "UnrealInventorySystem|InventoryUtilits")
    static FName GetDefaultInteractProfile();
};
