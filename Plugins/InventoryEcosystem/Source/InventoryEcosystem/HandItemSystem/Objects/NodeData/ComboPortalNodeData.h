// © 2025 mrbaconvn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h" 
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphData.h"
#include "ComboPortalNodeData.generated.h"

UCLASS()
class HANDITEMSYSTEM_API UComboPortalNodeData : public UComboGraphNodeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FGameplayTag PortalTag;
};
