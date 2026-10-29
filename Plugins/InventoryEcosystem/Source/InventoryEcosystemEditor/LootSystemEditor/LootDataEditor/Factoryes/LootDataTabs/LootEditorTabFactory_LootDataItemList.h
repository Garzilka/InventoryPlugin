#pragma once
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Item/List/W_ItemListView.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;
class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootDataItemList : public FWorkflowTabFactory
{
public:
	FLootEditorTabFactory_LootDataItemList(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);
                                        
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
                                        
private:
	TSharedPtr<SW_ItemListView> ItemListWidget;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};
