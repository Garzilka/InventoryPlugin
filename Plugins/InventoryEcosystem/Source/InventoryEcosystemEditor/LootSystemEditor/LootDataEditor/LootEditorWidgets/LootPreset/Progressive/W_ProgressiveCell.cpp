#include "W_ProgressiveCell.h"

#include "EditorStyleSet.h"
#include "PropertyHandle.h"
#include "W_ProgressiveCategory.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<SW_BaseArrayElement> SW_AC_ProgressiveCellArray::CreateArrayElement(
	TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
	return SNew(SW_ProgressiveCategory)
		.LootAssetEditorRef(LootAssetEditor)
		.PropertyHandle_Struct(InPropertyHandle_Array->AsArray()->GetElement(Idx))
		.ElementIdx(Idx);
}

void SW_ProgressiveCell::Construct(const FArguments& InArgs)
{
	PropertyHandle_Array = InArgs._PropertyHandle_Array;
	PropertyHandle_Key = InArgs._PropertyHandle_Key;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	ElementIdx = InArgs._ElementIdx;
	
	ChildSlot
	[
		SNew(SBox)
		.Padding(2)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
				.Padding(3.f)
				[
					SNew(SBox)
					[
						SNew(SW_AC_ProgressiveCellArray)
						.ArrayRef(PropertyHandle_Array)
						.DisplayName(PropertyHandle_Array->GetPropertyDisplayName())
						.CustomWidget(PropertyHandle_Key->CreatePropertyValueWidget())
						.PropertyHandle_OwnerStruct(PropertyHandle_Array)
						.LootAssetEditorRef(LootAssetEditor)
					]
				]
			]
		]
	];
}