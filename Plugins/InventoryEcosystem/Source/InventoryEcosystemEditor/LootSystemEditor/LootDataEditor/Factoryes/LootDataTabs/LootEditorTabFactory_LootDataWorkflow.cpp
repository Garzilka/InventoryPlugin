#include "LootEditorTabFactory_LootDataWorkflow.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"


FLootEditorTabFactory_LootDataWorkflow::FLootEditorTabFactory_LootDataWorkflow(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootData_Workflow, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Workflow"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;
	LootDataMainWidget = SNew(SLootDataMainWidget).LootAssetEditorRef(LootAssetEditor);
	LootAssetEditor.Pin()->OnNewSelectedLootDataObject.BindRaw(this, &FLootEditorTabFactory_LootDataWorkflow::NewObject);
}

TSharedRef<SWidget> FLootEditorTabFactory_LootDataWorkflow::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!LootAssetEditor.IsValid()) return SNew(STextBlock).Text(FText::FromString("Error Tab"));
	return LootDataMainWidget.ToSharedRef();
}

FText FLootEditorTabFactory_LootDataWorkflow::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}

void FLootEditorTabFactory_LootDataWorkflow::NewObject(FName NewObject)
{
	LootDataMainWidget->Init(NewObject);
}
