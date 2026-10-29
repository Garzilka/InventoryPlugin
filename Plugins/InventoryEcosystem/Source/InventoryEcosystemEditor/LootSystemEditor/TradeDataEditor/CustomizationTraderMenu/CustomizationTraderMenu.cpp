#include "CustomizationTraderMenu.h"

#include "DetailWidgetRow.h"
#include "EditorStyleSet.h"
#include "IDetailChildrenBuilder.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_DeleteSingle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/TradeDataEditor/Widgets/CustomizationCurrencyByScore/CustomizationCurrencyByScore.h"
#include "InventoryEcosystemEditor/LootSystemEditor/TradeDataEditor/Widgets/TradeType/TradeType.h"
#include "Widgets/Layout/SExpandableArea.h"

TSharedRef<IPropertyTypeCustomization> FCustomizationTraderMenu::MakeInstance()
{
	return MakeShareable(new FCustomizationTraderMenu());
}

void FCustomizationTraderMenu::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FCustomizationTraderMenu::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!StructPropertyHandle->IsValidHandle()) return;

	PropertyHandle_Struct =			StructPropertyHandle;
	PropertyHandle_bIsExpended =	PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyTraderData, bIsExpended));
	PropertyHandle_TradeType =		PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyTraderData, TradeType));
	PropertyHandle_Array =			PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyTraderData, CurrencyValue));
		
	check(PropertyHandle_Array.IsValid());
	if (!PropertyHandle_Array.IsValid()) return;
	
	bool LbIsExpended = false;
	PropertyHandle_bIsExpended->GetValue(LbIsExpended);
	StructBuilder.AddCustomRow(FText::FromString("Panel"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(0.f, 5.f, 0.f, 5.f)
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
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
							PropertyHandle_Struct->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.Padding(5.f, 0.f, 0.f, 0.f)
						.AutoWidth()
						[
							SNew(STradeType)
							.StructPropertyHandle(PropertyHandle_TradeType)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_Add)
								.OnClicked(this, &FCustomizationTraderMenu::SIGNATURE_BTN_ADD)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_DeleteSingle)
							.OnClicked(this, &FCustomizationTraderMenu::SIGNATURE_BTN_CLEAR)
						]
					]
					.BodyContent()
					[
						MakeArrayWidget()
					]
				]
			]
			
		];
}


EVisibility FCustomizationTraderMenu::GetVisibilityArray() const
{
	return (Widget_Expand.Get() && Widget_Expand.Get()->IsExpanded() ? EVisibility::Visible : EVisibility::Collapsed);
}
TSharedRef<SWidget> FCustomizationTraderMenu::MakeArrayWidget()
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
		Widget_Currencies = SNew(SVerticalBox);
	}
	
	for (uint32 childIndex = 0; childIndex < childrenCount; childIndex++)
	{
		AddChieldAt(childIndex);
	}
	return Widget_Currencies.ToSharedRef();
}

void FCustomizationTraderMenu::AddChieldAt(int32 Idx)
{	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	if (!(Idx > INDEX_NONE && Idx < (int32)childrenCount)) return;
	
	auto elementHandle = PropertyHandle_Array->GetChildHandle(Idx);
	Widget_Currencies->AddSlot()
	.AutoHeight()	  
	.Padding(2.f, 4.f, 2.f, 4.f)
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
				SNew(SCustomizationCurrencyByScore)
				.StructPropertyHandle(elementHandle)
				.RowID(Idx)
				.OnClicked(this, &FCustomizationTraderMenu::EVENT_REMOVE_SCORE)
			]
		]
	];	
}

FReply FCustomizationTraderMenu::SIGNATURE_BTN_VIEW()
{
	PropertyHandle_bIsExpended->SetValue((Widget_Expand.Get() && Widget_Expand.Get()->IsExpanded()));
	return FReply::Handled();
}

FReply FCustomizationTraderMenu::SIGNATURE_BTN_ADD()
{
	if (!PropertyHandle_Array.IsValid()) return FReply::Handled();

	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	arrayPropertyHandle->AddItem();
	
	MakeArrayWidget();

	return FReply::Handled();
}

FReply FCustomizationTraderMenu::SIGNATURE_BTN_CLEAR()
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

void FCustomizationTraderMenu::EVENT_REMOVE_SCORE(int32 Idx)
{
	if (!PropertyHandle_Array.IsValid()) return;
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	if (Idx < 0) return;

	arrayPropertyHandle->DeleteItem(Idx);	
	MakeArrayWidget();
}
