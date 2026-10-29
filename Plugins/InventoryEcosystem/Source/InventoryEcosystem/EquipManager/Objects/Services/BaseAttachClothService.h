// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/EquipManager/Data/CharacterMeshData.h"
#include "BaseAttachClothService.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class EQUIPMANAGER_API UBaseAttachClothService : public UBaseItemService
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Одежда", meta = (DisplayName = "Тип тела"))
	FGameplayTag BodyTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InstallTag;

	UPROPERTY(EditDefaultsOnly, Category = "Слоты экипировки", meta = (DisplayName = "Открыть слоты для экипировки"))
	FGameplayTagContainer OpenEquipSlots;

	UPROPERTY(EditDefaultsOnly, Category = "Слоты экипировки", meta = (DisplayName = "Закрыть слоты для экипировки"))
	FGameplayTagContainer CloseEquipSlots;

	UPROPERTY(BlueprintReadWrite, Category = "Одежда", EditAnywhere)
	TMap<FName, FClothData> ClothGenderData;

	UPROPERTY(BlueprintReadWrite, Category = "Одежда", EditAnywhere)
	TMap<FName, TSoftObjectPtr<USkeletalMesh>> GenderData;

public:
	UFUNCTION(BlueprintPure)
	FGameplayTag GetInstallTag() { return InstallTag; }

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetOpenEquipSlots() { return OpenEquipSlots; };

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetCloseEquipSlots() { return CloseEquipSlots; };

	UFUNCTION(BlueprintPure)
	FClothData GetMeshData(FName Gender);

	UFUNCTION(BlueprintPure)
	FGameplayTag GetBodyTag() { return BodyTag; };
};
