#include "W_RandomGenerationData.h"

#include "W_RandomGenerationCell.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/AmountGenerateSettings/W_AmountGenerateSettings.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<SW_BaseArrayElement> SW_AC_RandomGeneration::CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array,
	int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
	return SNew(SW_RandomGenerationCell)
			.LootAssetEditorRef(LootAssetEditor)
			.ElementIdx(Idx)
			.PropertyHandle_Struct(InPropertyHandle_Array->AsArray()->GetElement(Idx));
}

void SW_RandomGenerationData::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	if (!(PropertyHandle_Struct && PropertyHandle_Struct.IsValid())) return;
	
	PropertyHandle_ArrayCells = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRandomItemsData, RandomItemsData));
	if (!PropertyHandle_ArrayCells.IsValid()) return;

	ChildSlot
	[
		SNew(SW_AC_RandomGeneration)
		.EdgingColor(FLinearColor(0.0f, 0.0f, 0.0f))
		.BackgroundColor(FLinearColor(0.0f, 0.3f, 0.3f))
		.ArrayRef(PropertyHandle_ArrayCells)
		.LootAssetEditorRef(LootAssetEditor)
		.DisplayName(PropertyHandle_ArrayCells->GetPropertyDisplayName())
	];
}