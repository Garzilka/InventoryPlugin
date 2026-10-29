// © 2025 mrbaconvn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphData.h"
#include "ComboExecutionNodeData.generated.h"

class UComboActionPass;
class UComboManagerComponent;

UCLASS()
class HANDITEMSYSTEM_API UComboExecutionNodeData : public UComboGraphNodeData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly, Category = "Node Data")
	TArray<TObjectPtr<UComboActionPass>> ExecutionPasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node Data")
	bool bBreakWhenPassFailed = false;

public: 
	virtual const UComboGraphNodeData* ExecuteNode(UComboGraphInstance* GraphInstance) const override;
};
