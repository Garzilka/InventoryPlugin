// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NativeGameplayTags.h"
#include "InventoryData.generated.h"


namespace InventoryItemType
{
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Module);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cloth);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Medicine);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Food);
	ITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Currency);
}

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMenuAction : uint8
{
	None =			0 UMETA(Hidden),
	Equip =			1 << 0  UMETA(DisplayName = "Экипировать"),
	Use =			1 << 1	UMETA(DisplayName = "Использовать"),
	Drop =			1 << 2  UMETA(DisplayName = "Выбросить"),
	Modificated =	1 << 3  UMETA(DisplayName = "Модифицировать"),
	divide =		1 << 4  UMETA(DisplayName = "Разделить"),
	Unlock =        1 << 5  UMETA(DisplayName = "Открыть"),
};

USTRUCT(BlueprintType)
struct FDefaultItemData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Имя предмета"))
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true, DisplayName = "Описание предмета"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Основные", meta = (Bitmask, BitmaskEnum = "/Script/InventorySystem.EMenuAction", DisplayName = "Что можно делать с предметом"))
	int32 ActionMenu = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Тип предмета"))
	FGameplayTag ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Редкость предмета"))
	FGameplayTag RarityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsStack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bIsStack"))
	int32 StackSize = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Количество позиций"))
	FIntPoint Dimensions = FIntPoint(1);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", DisplayName = "Вес предмета", ToolTip = "Вес единицы предмета в граммах"))
	int32 Weight = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemLVL = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", DisplayName = "Action текст"))
	FDataTableRowHandle ActionText;
};

USTRUCT(BlueprintType)
struct FActionText : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EMenuAction, FText> ActionText;
};

UCLASS()
class ITEMSYSTEM_API UInventoryData : public UDataAsset
{
	GENERATED_BODY()	
};
