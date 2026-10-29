// © 2025 mrbaconvn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h" 
#include "GameplayTagContainer.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphData.h"
#include "ComboJumpNodeData.generated.h"

UCLASS()
class HANDITEMSYSTEM_API UComboJumpNodeData : public UComboGraphNodeData
{
	GENERATED_BODY()

public:
	virtual const UComboGraphNodeData* ExecuteNode(UComboGraphInstance* GraphInstance) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	FGameplayTag PortalTag;
};
