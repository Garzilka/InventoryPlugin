#pragma once
#include "CoreMinimal.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/W_LootPresetMainWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;
class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootPresetWorkflow : public FWorkflowTabFactory
{
public:
	void NewObject(FName Name) const;
	FLootEditorTabFactory_LootPresetWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TSharedPtr<SW_LootPresetMainWidget> LootPresetWidget;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};
