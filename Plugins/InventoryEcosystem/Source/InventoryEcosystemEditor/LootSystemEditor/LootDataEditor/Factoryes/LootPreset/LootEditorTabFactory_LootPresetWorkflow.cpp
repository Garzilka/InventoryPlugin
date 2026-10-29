#include "LootEditorTabFactory_LootPresetWorkflow.h"

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"


FLootEditorTabFactory_LootPresetWorkflow::FLootEditorTabFactory_LootPresetWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootPreset_Workflow, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Workflow"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;
	LootPresetWidget = SNew(SW_LootPresetMainWidget).LootAssetEditorRef(LootAssetEditor);
	LootAssetEditor.Pin()->OnNewSelectedLootPressetObject.BindRaw(this, &FLootEditorTabFactory_LootPresetWorkflow::NewObject);
}

TSharedRef<SWidget> FLootEditorTabFactory_LootPresetWorkflow::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!LootAssetEditor.IsValid()) return SNew(STextBlock).Text(FText::FromString("Error Tab"));
	return LootPresetWidget.ToSharedRef();
}

FText FLootEditorTabFactory_LootPresetWorkflow::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}
void FLootEditorTabFactory_LootPresetWorkflow::NewObject(FName NewObject) const
{
	LootPresetWidget->Init(NewObject);
}