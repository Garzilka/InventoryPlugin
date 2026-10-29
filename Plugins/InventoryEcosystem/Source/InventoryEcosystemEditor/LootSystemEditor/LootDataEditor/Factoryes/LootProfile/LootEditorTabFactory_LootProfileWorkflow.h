#pragma once
#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;

class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootProfileWorkflow : public FWorkflowTabFactory
{
public:

	FLootEditorTabFactory_LootProfileWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:

	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};

