// © 2025 mrbaconvn. All Rights Reserved.


#include "ComboAbilityDataPayload.h"

FString FAnimAbilityDataPayload::GetPreviewText() const
{
	return Super::GetPreviewText()
		+ FString::Printf(TEXT("\nMontage: %s"), *CombatMontage.GetAssetName());
}

TArray<FSoftObjectPath> FAnimAbilityDataPayload::GetPreviewAssets() const
{
	return {CombatMontage.ToSoftObjectPath()};
}
