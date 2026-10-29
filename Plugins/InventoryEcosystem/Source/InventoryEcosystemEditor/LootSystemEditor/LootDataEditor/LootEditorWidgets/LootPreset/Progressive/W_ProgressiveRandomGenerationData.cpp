#include "W_ProgressiveRandomGenerationData.h"


#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/AmountGenerateSettings/W_AmountGenerateSettings.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/RandomGeneration/W_RandomGenerationCell.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<SW_BaseArrayElement> SW_AC_ProgressiveRandom::CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array,
	int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
	return SNew(SW_RandomGenerationCell)
			.LootAssetEditorRef(LootAssetEditor)
			.PropertyHandle_Struct(InPropertyHandle_Array->AsArray()->GetElement(Idx))
			.ElementIdx(Idx);
}

TSharedRef<SWidget> SW_AC_ProgressiveRandom::CreateAdditional()
{
	if (!(PropertyHandle_OwnerStruct && PropertyHandle_OwnerStruct.IsValid())) return SNew(SBox);

	TSharedPtr<IPropertyHandle> PropertyHandle_AmountSettings	= PropertyHandle_OwnerStruct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRandomItemsDataAmount, AmountGenerateSettings));
	if (!PropertyHandle_AmountSettings.IsValid()) return SNew(SBox);

	return SNew(SW_AmountGenerateSettings)
		.LootAssetEditorRef(LootAssetEditor)
		.PropertyHandle_Struct(PropertyHandle_AmountSettings);
}

void SW_ProgressiveRandomGenerationData::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	
	
	PropertyHandle_ArrayCells = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRandomItemsDataAmount, RandomRowsData));
	if (!PropertyHandle_ArrayCells.IsValid()) return;

	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(0.f, 0.f, 0.f, 5.f)
		.AutoHeight()
		[
			SNew(SW_AC_ProgressiveRandom)
			.EdgingColor(FLinearColor(0.0f, 0.0f, 0.0f))
			.BackgroundColor(FLinearColor(0.0f, 0.3f, 0.3f))
			.LootAssetEditorRef(LootAssetEditor)
			.ArrayRef(PropertyHandle_ArrayCells)
			.DisplayName(PropertyHandle_ArrayCells->GetPropertyDisplayName())
			.PropertyHandle_OwnerStruct(PropertyHandle_Struct)
		]
	];
}
