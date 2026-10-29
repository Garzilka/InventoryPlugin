#pragma once
#include "CoreMinimal.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootSettingWidget/LootSettingMainWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FLootEditorAssetEditor;

class LOOTSYSTEMEDITOR_API FLootEditorTabFactory_LootSettingsWorkflow : public FWorkflowTabFactory
{
public:

	FLootEditorTabFactory_LootSettingsWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditorAsset);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	UFUNCTION()
	void NewObject(FName NewObject);
private:
	TSharedPtr<SLootSettingMainWidget> LootSettingsMainWidget;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
};