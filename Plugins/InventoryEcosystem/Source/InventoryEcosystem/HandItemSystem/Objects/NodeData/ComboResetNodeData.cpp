// © 2025 mrbaconvn. All Rights Reserved.


#include "ComboResetNodeData.h"

#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

const UComboGraphNodeData* UComboResetNodeData::ExecuteNode(UComboGraphInstance* GraphInstance) const
{
	GraphInstance->ResetInstance(EComboResetReason::END_COMBO);
	return nullptr;
}
