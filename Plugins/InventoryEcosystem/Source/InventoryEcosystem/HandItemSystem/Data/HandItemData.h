// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/ItemSystem/Library/InventoryLibrary.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "HandItemData.generated.h"

USTRUCT()
struct FHandItemManagerSlotSave
{
	GENERATED_BODY()

	FGameplayTag SlotTag;
	int32 NumArray = 0;
	TSubclassOf<UBaseItem> SavedClass;
	TArray<uint8> ItemData = {};

	FHandItemManagerSlotSave() {};
	FHandItemManagerSlotSave(UBaseItem* Item, FGameplayTag InSlotTag);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FHandItemManagerSlotSave& Value)
	{
		if (Ar.IsSaving())
		{
			Ar << Value.SavedClass;
			Ar << Value.SlotTag;
			Value.NumArray = Value.ItemData.Num();
			Ar << Value.NumArray;
			Ar << Value.ItemData;
		}

		if (Ar.IsLoading())
		{
			Ar << Value.SavedClass;
			Ar << Value.SlotTag;
			Ar << Value.NumArray;
			Value.ItemData.SetNum(Value.NumArray);
			Ar << Value.ItemData;
		}
		return Ar;
	}

};

USTRUCT()
struct FHandItemManagerSave
{
	GENERATED_BODY()

	int32 Saved = INDEX_NONE;
	int32 NumArray = 0;
	TArray<FHandItemManagerSlotSave> SavedData;

	bool IsValidData() { return (Saved > INDEX_NONE); };

public:
	FHandItemManagerSave() {};
	FHandItemManagerSave(class UBaseHandItemManager* EquipManager);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FHandItemManagerSave& Value)
	{
		if (Ar.IsSaving())
		{
			Ar << Value.Saved;
			Value.NumArray = Value.SavedData.Num();
			Ar << Value.NumArray;
			Ar << Value.SavedData;
		}

		if (Ar.IsLoading())
		{
			Ar << Value.Saved;
			Ar << Value.NumArray;
			Value.SavedData.SetNum(Value.NumArray);
			Ar << Value.SavedData;
		}
		return Ar;
	}
};

UENUM(BlueprintType)
enum class EEquipNotifyType : uint8
{
	EENT_None		UMETA(Hidden),
	EENT_Equip		UMETA(DisplayName = "Взять"),
	EENT_Unequip	UMETA(DisplayName = "Убрать"),
};

UENUM(BlueprintType)
enum class EEquipStateType : uint8
{
	EENT_None				UMETA(DisplayName = "None"),
	EENT_Equip				UMETA(DisplayName = "Взять"),
	EENT_Unequip			UMETA(DisplayName = "Убрать"),
	EENT_UnequipToEquip		UMETA(DisplayName = "Заменить"),
};



USTRUCT(BlueprintType)
struct FHandItemState
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBaseItem* CurrentItem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBaseItem* TargetItem = nullptr;
};


USTRUCT(BlueprintType)
struct FCurrentActorData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	EEquipStateType CurrentEquipState;

	UPROPERTY(BlueprintReadOnly)
	AActor* Ref = nullptr;
};

USTRUCT(BlueprintType)
struct FEquipAnimation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Анмиация взятия"))
	UAnimMontage* EquipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Анмиация убирания"))
	UAnimMontage* UnequipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Анмиация взятия (предмет)"))
	UAnimMontage* EquipAnimationItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Анмиация убирания (предмет)"))
	UAnimMontage* UnequipAnimationItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Сокет экипировки предмета"))
	FName EquipSocketAttach = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Относительное смещение экипированного предмета"))
	FTransform EquipSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Относительное смещение убранного предмета"))
	FTransform UnequipSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Сокет убирания предмета"))
	FName UnequipSocketAttach = NAME_None;

};

USTRUCT(BlueprintType)
struct FEquipSlotData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayPriority = -1, DisplayName = "Сокет убирания предмета"))
	FName UnequipSocketAttach = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Допустимые типы предметов"))
	FGameplayTagContainer ValidTypeEquip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow, DisplayName = "Анмиация к классу предмета"))
	TMap<TSubclassOf<AActor>, FEquipAnimation> EquipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow, DisplayName = "Анмиация к классу предмета"))
	TMap<TSubclassOf<UBaseItem>, FEquipAnimation> EquipAnimations;

};

USTRUCT(BlueprintType)
struct FHandSlotSetting
{
    GENERATED_BODY()

public:

    UPROPERTY(NotReplicated, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Предмет по умолчанию"))
    FInventoryItemData ItemSetting;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Тип слота"))
    FGameplayTag SlotType;

    UPROPERTY(BlueprintReadOnly)
    class UBaseItem* ItemRef = nullptr;

    UPROPERTY(BlueprintReadOnly)
    class ABaseItemActor* ActorRef = nullptr;

};

USTRUCT(BlueprintType)
struct FHandSlots
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FHandSlotSetting> Slots;
};



class HANDITEMSYSTEM_API HandItemData
{
public:
	HandItemData();
	~HandItemData();
};
