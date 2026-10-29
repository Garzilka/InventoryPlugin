#include "LootEditorAppMode_LootData.h"

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootDataTabs/LootEditorTabFactory_LootDataItemList.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootDataTabs/LootEditorTabFactory_LootDataList.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootDataTabs/LootEditorTabFactory_LootDataWorkflow.h"


FLootEditorAppMode_LootData::FLootEditorAppMode_LootData(TSharedPtr<FLootEditorAssetEditor> InLootEditor, FName InApplicationMode, TSharedPtr<FTabManager::FLayout> InLayout)
: FApplicationMode(InApplicationMode)
{
	LootEditor = InLootEditor;

	LootEditorTabFactories.RegisterFactory(MakeShareable(new FLootEditorTabFactory_LootDataList(InLootEditor.ToSharedRef())));
	LootEditorTabFactories.RegisterFactory(MakeShareable(new FLootEditorTabFactory_LootDataWorkflow(InLootEditor.ToSharedRef())));
	LootEditorTabFactories.RegisterFactory(MakeShareable(new FLootEditorTabFactory_LootDataItemList(InLootEditor.ToSharedRef())));

	TabLayout = InLayout;
}

void FLootEditorAppMode_LootData::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(LootEditor.IsValid());
	TSharedPtr<FLootEditorAssetEditor> LootEditorAssetEditorPtr = LootEditor.Pin();
	LootEditorAssetEditorPtr->RegisterToolBarTab(InTabManager.ToSharedRef());
	LootEditorAssetEditorPtr->PushTabFactories(LootEditorTabFactories);
	// FApplicationMode::RegisterTabFactoriesWithManager(InTabManager);
}
