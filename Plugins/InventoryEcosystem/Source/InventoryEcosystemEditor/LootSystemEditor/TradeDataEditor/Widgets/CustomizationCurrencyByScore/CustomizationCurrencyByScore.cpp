#include "CustomizationCurrencyByScore.h"

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_DeleteSingle.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Tools/AdvancedVerticalBox.h"
#include "InventoryEcosystemEditor/LootSystemEditor/TradeDataEditor/Widgets/CurrencyValue/CurrencyValue.h"
#include "Widgets/Layout/SBox.h"


void SCustomizationCurrencyByScore::Construct(const FArguments& InArgs)
{	
	PropertyHandle_Struct = InArgs._StructPropertyHandle;	
	RowID = InArgs._RowID;
	OnClicked = InArgs._OnClicked;
	if (!PropertyHandle_Struct.IsValid()) return;

	PropertyHandle_bIsExpended = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValueByScore, bIsExpended));
	PropertyHandle_Score = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValueByScore, Score));
	PropertyHandle_Array = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValueByScore, CurrencyValues));


	ChildSlot
	[
	SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(5.f, 0.f, 0.f, 0.f)
	.AutoWidth()
	[
		SAssignNew(Widget_Expand, SExpandableArea)
		.InitiallyCollapsed(true)
		.HeaderContent()
		[
			
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(5.f, 0.f, 0.f, 0.f)
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50.f)
				[
					PropertyHandle_Score->CreatePropertyValueWidget()
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f, 0.f, 0.f)
			[
				SNew(SW_BTN_Add)
					.OnClicked(this, &SCustomizationCurrencyByScore::SIGNATURE_BTN_ADD)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f, 0.f, 0.f)
			[
				SNew(SW_BTN_DeleteSingle)
				.OnClicked(this, &SCustomizationCurrencyByScore::SIGNATURE_BTN_REMOVE)
			]
			]
			.BodyContent()
			[
				MakeArrayWidget()
			]
		]
	];
}

FReply SCustomizationCurrencyByScore::SIGNATURE_BTN_VIEW() const
{
	PropertyHandle_bIsExpended->SetValue((Widget_Expand.Get() && Widget_Expand.Get()->IsExpanded()));
	return FReply::Handled();
}

FReply SCustomizationCurrencyByScore::SIGNATURE_BTN_ADD()
{
	if (!PropertyHandle_Array.IsValid()) return FReply::Handled();

	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	arrayPropertyHandle->AddItem();
	MakeArrayWidget();
	
	return FReply::Handled();
}

FReply SCustomizationCurrencyByScore::SIGNATURE_BTN_CLEAR()
{
	if (!PropertyHandle_Array.IsValid()) return FReply::Handled();
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	arrayPropertyHandle->EmptyArray();
	
	if (Widget_Currencies.Get())
	{
		Widget_Currencies->ClearChildren();
	}
	else
	{
		MakeArrayWidget();
	}
	
	return FReply::Handled();
}

FReply SCustomizationCurrencyByScore::SIGNATURE_BTN_REMOVE()
{
	OnClicked.ExecuteIfBound(RowID);	
	return FReply::Handled();
}

void SCustomizationCurrencyByScore::Event_RemoveCurrency(int32 Idx)
{
	if (!PropertyHandle_Array.IsValid()) return;
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	if (Idx < 0) return;

	arrayPropertyHandle->DeleteItem(Idx);	
	MakeArrayWidget();
}

TSharedRef<SWidget> SCustomizationCurrencyByScore::MakeArrayWidget()
{
	if (!PropertyHandle_Array.IsValid()) return SNew(SVerticalBox);
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);	

	if (Widget_Currencies.Get())
	{
		Widget_Currencies->ClearChildren();
	}
	else
	{
		Widget_Currencies = SNew(SAdvancedVerticalBox);
	}
	
	for (uint32 childIndex = 0; childIndex < childrenCount; childIndex++)
	{
		AddChieldAt(childIndex);
	}
	return Widget_Currencies.ToSharedRef();
}

void SCustomizationCurrencyByScore::AddChieldAt(int32 Idx)
{
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	if (!(Idx > INDEX_NONE && Idx < (int32)childrenCount)) return;
	
	auto elementHandle = PropertyHandle_Array->GetChildHandle(Idx);
	Widget_Currencies->AddSlot()
	.AutoHeight()
	.Padding(2.f)
	[
		SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0, 0, 0))
		.Content()
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0, 0.8f, 1.0f))
			.Content()
			[
				SNew(SCurrencyValue)
				.StructPropertyHandle(elementHandle)
				.RowID(Idx)
				.OnClicked(this, &SCustomizationCurrencyByScore::Event_RemoveCurrency)
			]
		]
	];
}
