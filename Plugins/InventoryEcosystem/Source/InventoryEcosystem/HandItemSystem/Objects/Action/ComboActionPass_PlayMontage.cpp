// © 2025 mrbaconvn. All Rights Reserved.


#include "ComboActionPass_PlayMontage.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "InventoryEcosystem/InventoryCore/Library/InventoryCoreLibrary.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/ComboManagerComponent.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

TArray<FSoftObjectPath> UComboActionPass_PlayMontage::GetPreviewAssets() const
{
	if(IsValid(MontageToPlay))
	{
		return { FSoftObjectPath(MontageToPlay) };
	}
	return {};
}

FString UComboActionPass_PlayMontage::GetPassInformation_Implementation() const
{
	return FString::Format(TEXT("Play \"{0}\""), { IsValid(MontageToPlay) ? MontageToPlay->GetName() : "None" });
}

bool UComboActionPass_PlayMontage::ExecutePass_Implementation(UComboGraphInstance* GraphInstance) const
{
	if(!IsValid(GraphInstance) || !GraphInstance->IsValidInstance())
	{
		return false;
	}
	
	ACharacter* Character = Cast<ACharacter>(GraphInstance->GetManagerComponent()->GetOwner());
	if (!IsValid(Character))
	{
		USkeletalMeshComponent* Mesh = UInventoryCoreLibrary::GetInventoryMesh(GraphInstance->GetManagerComponent()->GetOwner());
		if (!Mesh) return false;
		if (!Mesh->GetAnimInstance()) return false;

		float const Duration = Mesh->GetAnimInstance()->Montage_Play(MontageToPlay, PlayRate);
		
		if (Duration > 0.f)
		{
			// Start at a given Section.
			if( SectionName != NAME_None )
			{
				Mesh->GetAnimInstance()->Montage_JumpToSection(SectionName, MontageToPlay);
			}
		}
		
		return true;
	}

	if (MontageToPlay)
	{
		Character->PlayAnimMontage(MontageToPlay, PlayRate, SectionName);
		return true;
	}
	return false;
} 