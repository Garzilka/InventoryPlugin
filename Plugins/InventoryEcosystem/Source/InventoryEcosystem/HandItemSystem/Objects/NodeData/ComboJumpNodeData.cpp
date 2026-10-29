// © 2025 mrbaconvn. All Rights Reserved.

#include "ComboJumpNodeData.h"

#include "ComboPortalNodeData.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphAsset/ComboGraphAsset.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/GraphInstance/ComboGraphInstance.h"

const UComboGraphNodeData* UComboJumpNodeData::ExecuteNode(UComboGraphInstance* GraphInstance) const
{
	if (!IsValid(GraphInstance) || !GraphInstance->IsValidInstance())
	{
		return nullptr;
	} 

	TArray<UComboPortalNodeData*> PortalNodes = GraphInstance->GetComboGraphAsset()->GraphData->GetListNodeData<UComboPortalNodeData>(); 
	for (UComboPortalNodeData* PortalNode : PortalNodes)
	{
		if (PortalNode->PortalTag == PortalTag)
		{
			return PortalNode;
		}
	} 

	// We can't find the portal node so it will stuck here
	return nullptr;
}
