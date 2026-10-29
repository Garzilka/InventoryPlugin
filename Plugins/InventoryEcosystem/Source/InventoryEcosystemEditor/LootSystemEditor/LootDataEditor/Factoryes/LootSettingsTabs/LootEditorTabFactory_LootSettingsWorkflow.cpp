#include "LootEditorTabFactory_LootSettingsWorkflow.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"


FLootEditorTabFactory_LootSettingsWorkflow::FLootEditorTabFactory_LootSettingsWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootSettings_Workflow, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Workflow"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;

	LootSettingsMainWidget = SNew(SLootSettingMainWidget).LootAssetEditorRef(LootAssetEditor);
	LootAssetEditor.Pin()->OnNewSelectedSettingsDataObject.BindRaw(this, &FLootEditorTabFactory_LootSettingsWorkflow::NewObject);
}

TSharedRef<SWidget> FLootEditorTabFactory_LootSettingsWorkflow::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!LootAssetEditor.IsValid()) return SNew(STextBlock).Text(FText::FromString("Error Tab"));
	return LootSettingsMainWidget.ToSharedRef();
}

FText FLootEditorTabFactory_LootSettingsWorkflow::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}

void FLootEditorTabFactory_LootSettingsWorkflow::NewObject(FName NewObject)
{
	LootSettingsMainWidget->Init(NewObject);
}
