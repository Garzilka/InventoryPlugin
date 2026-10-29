#include "W_Progressive.h"

#include "PropertyHandle.h"
#include "W_ProgressiveCell.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_StandardizedRow.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<SW_BaseArrayElement> SW_AC_Progressive::CreateArrayElement(
	TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
	TSharedRef<IPropertyHandle> LPropertyHandle_Pair = InPropertyHandle_Array->AsMap()->GetElement(Idx);		
	TSharedPtr<IPropertyHandle> LPropertyHandle_Array = LPropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveData, ProgressiveDataCells));

	return SNew(SW_ProgressiveCell)
		.LootAssetEditorRef(LootAssetEditor)
		.PropertyHandle_Key(LPropertyHandle_Pair->GetKeyHandle())
		.PropertyHandle_Array(LPropertyHandle_Array)
		.ElementIdx(Idx);
}

void SW_Progressive::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	
	
	PropertyHandle_GenerateRules = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveGenerationSettings, GenerateRules));
	if (!PropertyHandle_GenerateRules.IsValid()) return;
	
	PropertyHandle_ProgressiveData = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveGenerationSettings, ProgressiveData));
	if (!PropertyHandle_ProgressiveData.IsValid()) return;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(0.f, 0.f, 0.f, 5.f)
		.AutoHeight()
		[
			SNew(SW_StandardizedRow)
			.DisplayName(PropertyHandle_GenerateRules->GetPropertyDisplayName())
			.PropertyHandle(PropertyHandle_GenerateRules)
		] 
		+ SVerticalBox::Slot()
		.Padding(5.f, 5.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SW_AC_Progressive)
			.ArrayRef(PropertyHandle_ProgressiveData)
			.DisplayName(PropertyHandle_ProgressiveData->GetPropertyDisplayName())
			.PropertyHandle_OwnerStruct(PropertyHandle_Struct)
			.LootAssetEditorRef(LootAssetEditor)
		]
	];
}