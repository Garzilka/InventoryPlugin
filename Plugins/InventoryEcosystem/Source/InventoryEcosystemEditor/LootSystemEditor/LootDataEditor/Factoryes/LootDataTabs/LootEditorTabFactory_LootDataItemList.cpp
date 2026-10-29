#include "LootEditorTabFactory_LootDataItemList.h"

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/Layout/SScrollBox.h"

FLootEditorTabFactory_LootDataItemList::FLootEditorTabFactory_LootDataItemList(TSharedRef<FLootEditorAssetEditor> InLootEditor)
	:FWorkflowTabFactory(LootEditorTabs::LootData_ItemList, InLootEditor), LootAssetEditor(InLootEditor)
{
	TabLabel = FText::FromString(TEXT("Loot Data Item List"));
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.PickAsset");
	bIsSingleton = true;
	ItemListWidget = SNew(SW_ItemListView);
}

TSharedRef<SWidget> FLootEditorTabFactory_LootDataItemList::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	if (!LootAssetEditor.IsValid()) return SNew(STextBlock).Text(FText::FromString("Error Tab"));
	
	return SNew(SBox)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Vertical)
			.ScrollBarVisibility(EVisibility::Visible)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ItemListWidget.ToSharedRef()
				]
			]			
		];
}

FText FLootEditorTabFactory_LootDataItemList::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}