#pragma once
#include "CoreMinimal.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/LootDataMainWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;

class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootDataWorkflow : public FWorkflowTabFactory
{
public:

	FLootEditorTabFactory_LootDataWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	UFUNCTION()
	void NewObject(FName NewObject);
private:

	TSharedPtr<SLootDataMainWidget> LootDataMainWidget;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};
