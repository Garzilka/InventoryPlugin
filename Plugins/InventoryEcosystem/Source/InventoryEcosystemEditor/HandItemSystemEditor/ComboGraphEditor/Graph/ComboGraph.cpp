// © 2025 mrbaconvn. All Rights Reserved.


#include "ComboGraph.h"

#include "EdGraph/EdGraphPin.h"
#include "InventoryEcosystem/HandItemSystem/Objects/ComboGraph/ComboGraphData.h"
#include "Node/ComboNodeBase.h"

UEdGraphPin* UComboGraph::FindPinById(const FGuid& PinId) const
{
    for (UEdGraphNode* Node : Nodes)
    {
        for (UEdGraphPin* Pin : Node->Pins)
        {
            if (Pin->PinId == PinId)
            {
                return Pin;
            }
        }
    }

    return nullptr;
}

UComboNodeBase* UComboGraph::FindNodeByData(const UComboGraphNodeData* NodeData) const
{
    if(!IsValid(NodeData))
    {
        return nullptr;
    }

    for (UEdGraphNode* Node : Nodes)
    {
        UComboNodeBase* ComboNode = Cast<UComboNodeBase>(Node);
        if (IsValid(ComboNode) && ComboNode->NodeGuid == NodeData->NodeGuid)
        {
            return ComboNode;
        }
    }

    return nullptr;
}
