#include "LootEditorAppMode_LootPreset.h"

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootPreset/LootEditorTabFactory_LootPresetList.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootPreset/LootEditorTabFactory_LootPresetWorkflow.h"


FLootEditorAppMode_LootPreset::FLootEditorAppMode_LootPreset(TSharedPtr<FLootEditorAssetEditor> InLootEditor, FName InApplicationMode, TSharedPtr<FTabManager::FLayout> InLayout)
: FApplicationMode(InApplicationMode)
{
	LootEditor = InLootEditor;

	LootEditorTabFactories.RegisterFactory(MakeShareable(new FLootEditorTabFactory_LootPresetList(InLootEditor.ToSharedRef())));
	LootEditorTabFactories.RegisterFactory(MakeShareable(new FLootEditorTabFactory_LootPresetWorkflow(InLootEditor.ToSharedRef())));

	TabLayout = InLayout;
}

void FLootEditorAppMode_LootPreset::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(LootEditor.IsValid());
	TSharedPtr<FLootEditorAssetEditor> LootEditorAssetEditorPtr = LootEditor.Pin();
	LootEditorAssetEditorPtr->RegisterToolBarTab(InTabManager.ToSharedRef());
	LootEditorAssetEditorPtr->PushTabFactories(LootEditorTabFactories);
	// FApplicationMode::RegisterTabFactoriesWithManager(InTabManager);
}
