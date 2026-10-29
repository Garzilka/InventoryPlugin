// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItemService.h"
#include "BaseQualityService.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ITEMSYSTEM_API UBaseQualityService : public UBaseItemService
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Качество", meta = (ClampMin = "0", DisplayName = "Максимальное состояние"))
	float MaxQualityStatus = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Качество", meta = (ClampMin = "0", DisplayName = "Урон за вызов", 
		ToolTip="Получает урон при вызове, если урон не определяется из вне. Например прямой урон по игроку"))
	float DefaultDamagePerUse = 1.f;

	UPROPERTY(EditDefaultsOnly, /*Replicated,*/ Category = "Качество", meta = (ClampMin = "0", DisplayName = "Состояние предмета"))
	float QualityStatus = 100.f;

public:
	UFUNCTION(BlueprintPure)
	bool IsBroken() { return (GetQuality() < 0.01f); }

	UFUNCTION(BlueprintPure)
	float GetQuality(bool IsPercent = true) { return (IsPercent ? (QualityStatus / MaxQualityStatus) : QualityStatus); };

	UFUNCTION(BlueprintCallable)
	void SetQuality(const float NewValue, bool IsPercent = true) { QualityStatus = (IsPercent ? (MaxQualityStatus / 100.f * NewValue) : (NewValue)); QualityStatus = (QualityStatus >= 0 ? QualityStatus : 0.f); }

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float value)
	{
		if (!HasAuthority()) return;
		float Damage = (value < 0 ? DefaultDamagePerUse : value);
		QualityStatus -= Damage;
		QualityStatus = (QualityStatus < 0.f ? 0.f : QualityStatus);
	}
};
