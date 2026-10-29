#include "LootEditorTabFactory_LootPresetList.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Buttons/ListAddButton/W_ListAddButton.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Category/LootSettingsListWidget.h"
#include "Widgets/Layout/SScrollBox.h"

FLootEditorTabFactory_LootPresetList::FLootEditorTabFactory_LootPresetList(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootPreset_List, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Loot data list"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;
	Place = SNew(SVerticalBox);
	LootAssetEditor.Pin()->OnNeedRebuilLootPresset.BindRaw(this, &FLootEditorTabFactory_LootPresetList::RebuildList);
}

void FLootEditorTabFactory_LootPresetList::OnButtonClicked(SW_ListAddButton* Sw_ListAddButton) const
{
	FName GUID = FName(FGuid::NewGuid().ToString());
	FLootPressetData NewPreset;
	NewPreset.MetaData.Object_Name = FText::FromName(GUID);
	LootAssetEditor.Pin()->GetLootDataRef()->LootPresets.Add(GUID, NewPreset);
	LootAssetEditor.Pin()->RebuildCategoryData(ECategorySelector::Preset);
	LootAssetEditor.Pin()->OnNeedRebuilLootPresset.ExecuteIfBound();
	LootAssetEditor.Pin()->GetLootDataRef()->Modify();
}

TSharedRef<SWidget> FLootEditorTabFactory_LootPresetList::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!LootAssetEditor.IsValid()) return SNew(STextBlock).Text(FText::FromString("Error Tab"));
	RebuildList();
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				Place.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SW_ListAddButton)
			.OnClicked(this, &FLootEditorTabFactory_LootPresetList::OnButtonClicked)
		];
}

FText FLootEditorTabFactory_LootPresetList::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}

void FLootEditorTabFactory_LootPresetList::RebuildList() const
{
	ULootData* LootDataRef = LootAssetEditor.Pin()->GetLootDataRef();
	Place.Get()->ClearChildren();
	if (!LootDataRef)
	{
		Place->AddSlot()[SNew(STextBlock).Text(FText::FromString("LootDataRef Error"))];
	}

	for (auto CategoryData : LootAssetEditor.Pin()->CurrentCategoryLootPreset.Children)
	{
		FCategoryObjectData NextCategory = (CategoryData.Value.Get() != nullptr ? *CategoryData.Value.Get() : FCategoryObjectData());
		Place->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(1.f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.3f,0.3f,0.3f))
			[
				SNew(SLootSettingsListWidget)
				.CategoryData(NextCategory)
				.CategoryType(ECategorySelector::Preset)
				.CurrentPath(CategoryData.Key)
				.CurrentRoot(CategoryData.Key)
				.LootAssetEditorRef(LootAssetEditor)
			]
			
		];
	}
}
