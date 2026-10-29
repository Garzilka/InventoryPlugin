#include "FCustomizationItemCurrenciesData.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorStyleSet.h"
#include "IDetailChildrenBuilder.h"
#include "CurrencyByTradeType/CurrencyByTradeType.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_DeleteSingle.h"
#include "Widgets/Layout/SExpandableArea.h"


#define LOCTEXT_NAMESPACE "UITrade"


TSharedRef<IPropertyTypeCustomization> FCustomizationItemCurrenciesData::MakeInstance()
{
	return MakeShareable(new FCustomizationItemCurrenciesData());
}

void FCustomizationItemCurrenciesData::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FCustomizationItemCurrenciesData::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!StructPropertyHandle->IsValidHandle()) return;

	
	PropertyHandle_Array = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FItemCurrecyTradeData, CurrenciesData));
	PropertyHandle_bIsExpended = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FItemCurrecyTradeData, bIsExpended));
		
	check(PropertyHandle_Array.IsValid());
	if (!PropertyHandle_Array.IsValid()) return;
	
	bool LbIsExpended = false;
	PropertyHandle_bIsExpended->GetValue(LbIsExpended);

	

	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
	if (!TradeSettings) return;
	
	if (TradeSettings->TradeTypes.Num() > 0)
	{
		KeyValues.Reset();
		for (auto LCurrencyType : TradeSettings->TradeTypes)
		{
			KeyValues.Add(LCurrencyType);
		}
	}
	else
	{
		KeyValues.Reset();
		KeyValues.Add(TEXT("None"));
	}

	StructBuilder.AddCustomRow(FText::FromString("Panel"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
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
							PropertyHandle_Array->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_Add)
								.OnClicked(this, &FCustomizationItemCurrenciesData::SIGNATURE_BTN_ADD)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_DeleteSingle)
							.OnClicked(this, &FCustomizationItemCurrenciesData::SIGNATURE_BTN_CLEAR)
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

TSharedRef<SWidget> FCustomizationItemCurrenciesData::MakeArrayWidget()
{
	if (!PropertyHandle_Array.IsValid()) return SNew(SVerticalBox);
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);	

	if (Currencies.Get())
	{
		Currencies->ClearChildren();
	}
	else
	{
		Currencies = SNew(SVerticalBox);
	}
	
	for (uint32 childIndex = 0; childIndex < childrenCount; childIndex++)
	{
		AddChieldAt(childIndex);
	}
	return Currencies.ToSharedRef();
}

void FCustomizationItemCurrenciesData::AddChieldAt(int32 Idx)
{	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	if (!(Idx > INDEX_NONE && Idx < (int32)childrenCount)) return;
	
	auto elementHandle = PropertyHandle_Array->GetChildHandle(Idx);
	Currencies->AddSlot()
	.AutoHeight()	  
	.Padding(2.f, 0.f, 2.f, 5.f)
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
				SNew(SCurrencyByTradeType)
				.StructPropertyHandle(elementHandle)
			]
		]
	];	
}


FReply FCustomizationItemCurrenciesData::SIGNATURE_BTN_ADD()
{
	if (!PropertyHandle_Array.IsValid()) return FReply::Handled();

	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	arrayPropertyHandle->AddItem();
	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	AddChieldAt((childrenCount - 1));

	return FReply::Handled();
}

FReply FCustomizationItemCurrenciesData::SIGNATURE_BTN_CLEAR()
{
	if (!PropertyHandle_Array.IsValid()) return FReply::Handled();
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_Array->AsArray();
	arrayPropertyHandle->EmptyArray();
	
	if (Currencies.Get())
	{
		Currencies->ClearChildren();
	}
	else
	{
		MakeArrayWidget();
	}
	
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE