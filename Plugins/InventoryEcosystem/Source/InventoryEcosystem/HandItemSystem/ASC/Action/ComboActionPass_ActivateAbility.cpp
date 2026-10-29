// © 2025 mrbaconvn. All Rights Reserved.


#include "ComboActionPass_ActivateAbility.h"
#include "AbilitySystemGlobals.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/ComboManagerComponent.h"
#include "InventoryEcosystem/HandItemSystem/Data/AbilityData/AbilityContext.h"
#include "InventoryEcosystem/HandItemSystem/Objects/Ability/ContextAbilitySystemComponent.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

bool UComboActionPass_ActivateAbility::ExecutePass_Implementation(UComboGraphInstance* GraphInstance) const
{
	if (!IsValid(GraphInstance) || !GraphInstance->IsValidInstance())
	{
		return false;
	}

	UContextAbilitySystemComponent* ASC = Cast<UContextAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GraphInstance->GetManagerComponent()->GetOwner()));
	if (!IsValid(ASC))
	{
		return false;
	}
	
	return ASC->TryActivateContextAbility(AbilityPayload);
}

TArray<FSoftObjectPath> UComboActionPass_ActivateAbility::GetPreviewAssets() const
{
	if (AbilityPayload.IsValid())
	{
		return AbilityPayload.Get<FAbilityContext>().GetPreviewAssets();
	}
	return Super::GetPreviewAssets();
}

FString UComboActionPass_ActivateAbility::GetPassInformation_Implementation() const
{
	if(AbilityPayload.IsValid())
	{
		FAbilityContext GenericPayload = AbilityPayload.Get<FAbilityContext>();
		return GenericPayload.GetPreviewText();
	}
	
	return Super::GetPassInformation_Implementation();
}
