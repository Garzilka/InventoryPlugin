// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NativeGameplayTags.h"
#include "InventoryEcosystem/InventoryTools/Data/MergeMeshData.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "CharacterMeshData.generated.h"


USTRUCT()
struct FEquipManagerSlotSave
{
	GENERATED_BODY()

	TSubclassOf<UBaseItem> SavedClass;
	int32 NumArray = 0;
	TArray<uint8> ItemData;

public:
	FEquipManagerSlotSave() {};
	FEquipManagerSlotSave(UBaseItem* Item);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FEquipManagerSlotSave& Value)
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
struct FEquipManagerSave
{
	GENERATED_BODY()

public:

	int32 Saved = INDEX_NONE;
	int32 NumArray = 0;
	TArray<FEquipManagerSlotSave> SaveData;

	bool IsValidData() { return (Saved > INDEX_NONE); };

	FEquipManagerSave() {};
	FEquipManagerSave(class UBaseEquipManager* EquipManager);

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FEquipManagerSave& Value)
	{
		if (Ar.IsSaving())
		{
			Ar << Value.Saved;
			Value.NumArray = Value.SaveData.Num();
			Ar << Value.NumArray;
			Ar << Value.SaveData;
		}

		if (Ar.IsLoading())
		{
			Ar << Value.Saved;
			Ar << Value.NumArray;
			Value.SaveData.SetNum(Value.NumArray);
			Ar << Value.SaveData;
		}
		return Ar;
	}
};

USTRUCT(BlueprintType)
struct FMeshSkinDataAsset
{
	GENERATED_BODY()
		
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = bWithIcon, EditConditionHides))
	UTexture2D* IconElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> Mesh;
};

USTRUCT(BlueprintType)
struct FClothData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> MeshRef = TSoftObjectPtr<USkeletalMesh>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMaterialInfo> MaterialMap;
};



UCLASS(BlueprintType)
class EQUIPMANAGER_API UCharacterGenderData : public UDataAsset
{
	GENERATED_BODY()
		
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* GenderIcon;	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText GenderNameText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ForceInlineRow))
	TMap<FGameplayTag, FMeshSkinDataAsset> BodyData;

	/**That need for the widget**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	bool bIsEnable = true;
	
	/**That need for the widget**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	bool bIsNeedHidde = false;
};


USTRUCT(BlueprintType)
struct FCharacterEditorGenderTable : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCharacterGenderData* GenderData;

};
