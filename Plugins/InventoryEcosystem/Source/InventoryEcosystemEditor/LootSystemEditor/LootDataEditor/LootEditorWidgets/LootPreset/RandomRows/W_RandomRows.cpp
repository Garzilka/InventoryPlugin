#include "W_RandomRows.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/AmountGenerateSettings/W_AmountGenerateSettings.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/Picker/W_LootPicker.h"
#include "Widgets/Layout/SExpandableArea.h"

void SW_RandomRows::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;

	PropertyHandle_Rows	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRandomRowsData, Rows));
	if (!PropertyHandle_Rows.IsValid()) return;

	PropertyHandle_AmountSettings	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRandomRowsData, AmountGenerateSettings));
	if (!PropertyHandle_AmountSettings.IsValid()) return;

	ChildSlot
	[
		SAssignNew(EA_Rows, SExpandableArea)
		.InitiallyCollapsed(true)
		.HeaderContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				PropertyHandle_Rows->CreatePropertyNameWidget()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f, 0.f, 0.f)
			[
				SNew(SW_BTN_Add)
				.OnClicked(this, &SW_RandomRows::AddButtonClicked)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f, 0.f, 0.f)
			[
				SNew(SW_BTN_ClearAll)
			]
		]
		.BodyContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5.f)
			[
				CreateArray()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()					
			.Padding(5.f)
			[
				SNew(SW_AmountGenerateSettings)
				.LootAssetEditorRef(LootAssetEditor)
				.PropertyHandle_Struct(PropertyHandle_AmountSettings)
			]
		]
	];
}

FReply SW_RandomRows::AddButtonClicked()
{
	if (!PropertyHandle_Rows.IsValid()) return FReply::Handled();
	if (!PropertyHandle_Rows->AsArray().IsValid()) return FReply::Handled();
	PropertyHandle_Rows->AsArray()->AddItem();
	CreateArray();
	return FReply::Handled();
}

TSharedRef<SWidget> SW_RandomRows::CreateArray()
{
	if (!VB_Array.IsValid())
	{
		VB_Array.Reset();
		VB_Array = SNew(SVerticalBox);
	}
	else
	{
		VB_Array->ClearChildren();
	}

	if (!PropertyHandle_Rows.IsValid()) return VB_Array.ToSharedRef();
	if (!PropertyHandle_Rows->AsArray().IsValid()) return VB_Array.ToSharedRef();
	
	uint32 Size;
	PropertyHandle_Rows->AsArray()->GetNumElements(Size);

	for (uint32 i = 0; i < Size; i++)
	{
		auto LStruct = PropertyHandle_Rows->AsArray()->GetElement(i);

		VB_Array->AddSlot()
		.AutoHeight()
		[
			SNew(SBox)
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SNew(SW_LootPicker)
				.LootAssetEditorRef(LootAssetEditor)
				.PropertyHandle_Struct(LStruct)
			]
		];
	}
	
	return VB_Array.ToSharedRef();
}
