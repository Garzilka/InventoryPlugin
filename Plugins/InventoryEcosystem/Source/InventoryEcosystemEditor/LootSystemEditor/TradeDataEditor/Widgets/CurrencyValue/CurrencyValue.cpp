#include "CurrencyValue.h"

#include "DetailLayoutBuilder.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_DeleteSingle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/TradeDataEditor/Widgets/CurrencyType/CurrencyType.h"
#include "Widgets/Layout/SBox.h"


void SCurrencyValue::Construct(const FArguments& InArgs)
{	
	PropertyHandle_Struct = InArgs._StructPropertyHandle;	
	RowID = InArgs._RowID;
	OnClicked = InArgs._OnClicked;
	if (!PropertyHandle_Struct.IsValid()) return;

	PropertyHandle_CurrencyType = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValue, CurrencyType));
	PropertyHandle_CurrencyValue = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValue, Value));
	
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCurrencyType)
			.StructPropertyHandle(PropertyHandle_CurrencyType)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(10.f, 0.f, 0.f, 0.f)
		[
			SNew(SBox)
			.WidthOverride(50.f)
			[
				PropertyHandle_CurrencyValue->CreatePropertyValueWidget()
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(10.f, 0.f, 0.f, 0.f)
		[
			SNew(SW_BTN_DeleteSingle)
			.OnClicked(this, &SCurrencyValue::SIGNATURE_BTN_REMOVE)
		]
	];
}


FReply SCurrencyValue::SIGNATURE_BTN_REMOVE() const
{
	OnClicked.ExecuteIfBound(RowID);	
	return FReply::Handled();
}
