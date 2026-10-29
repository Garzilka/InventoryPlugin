// © 2025 mrbaconvn. All Rights Reserved.


#include "CombatAbility.h"

#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/ComboManagerComponent.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

UCombatAbility::UCombatAbility()
{ 
}

void UCombatAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if(!ActorInfo->AvatarActor.IsValid())
	{
		return;
	}
	 
	UComboManagerComponent* ComboManager = ActorInfo->AvatarActor.Get()->GetComponentByClass<UComboManagerComponent>();
	if(!IsValid(ComboManager))
	{
		return;
	}
	
	UComboGraphInstance* Instance = Cast<UComboGraphInstance>(ComboManager->GetCurrentInstance());
	if(!IsValid(Instance))
	{
		return;
	}
	
	Instance->ResetInstance();
} 