// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryEcosystem/ItemSystem/Objects/ItemServices/BaseItemService.h"
#include "InventoryEcosystem/HandItemSystem/Data/HandItemData.h"

#include "BaseHandItemService.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandItemObjectPlayAnimationSignature, class UBaseHandItemService*, CharacterInstigator, bool, IsEquip);


UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class HANDITEMSYSTEM_API UBaseHandItemService : public UBaseItemService
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FHandItemObjectPlayAnimationSignature OnHandItemObjectPlayAnimation;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceInlineRow, DisplayName = "Анимации экипировки"))
	TMap<FGameplayTag, FEquipAnimation> EquipData;
public:

	UFUNCTION(BlueprintPure)
	bool GetHandData(FGameplayTag Tag, FEquipAnimation& Result);

	virtual void PlayEquipAnimation(FEquipAnimation Data, bool IsEquip);

	UFUNCTION(BlueprintNativeEvent)
	void DoEquipLogic(APawn* Instigator);
	void DoEquipLogic_Implementation(APawn* Instigator) {};

	UFUNCTION(BlueprintNativeEvent)
	void DoUnequipLogic(APawn* Instigator);
	void DoUnequipLogic_Implementation(APawn* Instigator) {};
};
