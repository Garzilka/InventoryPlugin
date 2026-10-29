// © 2025 mrbaconvn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphData.h"
#include "ComboResetNodeData.generated.h"

class UComboGraphNodeData;

UCLASS()
class HANDITEMSYSTEM_API UComboResetNodeData : public UComboGraphNodeData
{
	GENERATED_BODY()

public:
	virtual const UComboGraphNodeData* ExecuteNode(UComboGraphInstance* GraphInstance) const override;
};
