#include "LootEditorTabFactory_LootSettingsList.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Buttons/ListAddButton/W_ListAddButton.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Category/LootSettingsListWidget.h"
#include "Widgets/Layout/SScrollBox.h"

FLootEditorTabFactory_LootSettingsList::FLootEditorTabFactory_LootSettingsList(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootSettings_List, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Loot settings list"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;

	Place = SNew(SVerticalBox);
	LootAssetEditor.Pin()->OnNeedRebuilSettingData.BindRaw(this, &FLootEditorTabFactory_LootSettingsList::RebuildList);
}



void FLootEditorTabFactory_LootSettingsList::OnButtonClicked(SW_ListAddButton* Sw_ListAddButton) const
{
	LootAssetEditor.Pin()->GetLootDataRef()->LootSettings.Reset();
	LootAssetEditor.Pin()->RebuildCategoryData(ECategorySelector::Settings);
	LootAssetEditor.Pin()->OnNeedRebuilSettingData.ExecuteIfBound();;
}

TSharedRef<SWidget> FLootEditorTabFactory_LootSettingsList::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
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
			.OnClicked(this, &FLootEditorTabFactory_LootSettingsList::OnButtonClicked)
		];
}

FText FLootEditorTabFactory_LootSettingsList::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}

void FLootEditorTabFactory_LootSettingsList::RebuildList() const
{
	ULootData* LootDataRef = LootAssetEditor.Pin()->GetLootDataRef();
	Place.Get()->ClearChildren();
	if (!LootDataRef)
	{
		Place->AddSlot()[SNew(STextBlock).Text(FText::FromString("LootDataRef Error"))];
	}

	for (auto CategoryData : LootAssetEditor.Pin()->CurrentCategoryLootSettings.Children)
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
				.CategoryType(ECategorySelector::Settings)
				.CurrentPath(CategoryData.Key)
				.CurrentRoot(CategoryData.Key)
				.LootAssetEditorRef(LootAssetEditor)
			]
			
		];
	}
}
