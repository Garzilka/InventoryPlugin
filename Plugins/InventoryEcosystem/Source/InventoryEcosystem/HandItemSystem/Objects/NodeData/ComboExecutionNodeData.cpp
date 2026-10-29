// © 2025 mrbaconvn. All Rights Reserved.

#include "ComboExecutionNodeData.h"

#include "InventoryEcosystem/HandItemSystem/Objects/Action/ComboActionPass.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

const UComboGraphNodeData* UComboExecutionNodeData::ExecuteNode(UComboGraphInstance* GraphInstance) const
{
	if (!IsValid(GraphInstance))
	{
		return nullptr;
	}
	
	bool bExecuteSuccess = true;
	for (UComboActionPass* ActionPass : ExecutionPasses)
	{
		if (!IsValid(ActionPass))
		{
			continue;
		}

		if (!ActionPass->ExecutePass(GraphInstance))
		{
			bExecuteSuccess = false;
			if (bBreakWhenPassFailed)
			{
				break;
			}
		}
	}

	return GetNextOutputNodeByPinIndex(bExecuteSuccess ? 0 : 1);
}
