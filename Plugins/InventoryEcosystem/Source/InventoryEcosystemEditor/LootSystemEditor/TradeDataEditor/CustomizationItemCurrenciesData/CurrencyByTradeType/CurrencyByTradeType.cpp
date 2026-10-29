#include "CurrencyByTradeType.h"

#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_DeleteSingle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/TradeDataEditor/Widgets/CurrencyValue/CurrencyValue.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"


void SCurrencyByTradeType::Construct(const FArguments& InArgs)
{	
	PropertyHandle_Struct = InArgs._StructPropertyHandle;
	
	
	if (!PropertyHandle_Struct.IsValid()) return;

	auto PropertyHandle_TradeTypeStructure = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyDataByTradeType, TradeType));
	PropertyHandle_TradeType = PropertyHandle_TradeTypeStructure->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTradeType, TradeType));
	PropertyHandle_ArrayCurrecies = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyDataByTradeType, CurrencyValue));
	PropertyHandle_bIsExpended = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyDataByTradeType, bIsExpended));
	
	bool LbIsExpended = false;
	PropertyHandle_bIsExpended->GetValue(LbIsExpended);

	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
	if (!TradeSettings) return;


	if (TradeSettings->TradeTypes.Num() > 0)
	{
		KeyValues.Reset();
		KeyValues = TradeSettings->TradeTypes;
	}
	else
	{
		KeyValues.Reset();
		KeyValues.Add(TEXT("None"));
	}

	
	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(300)
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
					.AutoWidth()
					.Padding(10.f, 0.f, 0.f, 0.f)
					[
						SNew(SBox)
						.MinDesiredWidth(150)
						[
							SNew(SComboButton)
							.OnGetMenuContent(this, &SCurrencyByTradeType::OnGetKeyContent)
							.ContentPadding(FMargin(2.0f, 2.0f))
							.ButtonContent()
							[
								SNew(STextBlock)
									.Text(this, &SCurrencyByTradeType::GetCurrentKeyDesc)
									.Font(IDetailLayoutBuilder::GetDetailFont())
							]
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(10.f, 0.f, 0.f, 0.f)
					[
						SNew(SW_BTN_Add)
							.OnClicked(this, &SCurrencyByTradeType::SIGNATURE_BTN_ADD)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(10.f, 0.f, 0.f, 0.f)
					[
						SNew(SW_BTN_DeleteSingle)
						.OnClicked(this, &SCurrencyByTradeType::SIGNATURE_BTN_CLEAR)
					]
				]
				.BodyContent()
				[
					MakeArrayWidget()
				]
			]
		]
	];
	
	FName CurrentValue;
	PropertyHandle_TradeType->GetValue(CurrentValue);
	if (CurrentValue == NAME_None)
	{
		PropertyHandle_TradeType->SetValue(KeyValues[0]);
	}
}

TSharedRef<SWidget> SCurrencyByTradeType::MakeArrayWidget()
{
	if (!PropertyHandle_ArrayCurrecies.IsValid()) return SNew(SVerticalBox);
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_ArrayCurrecies->AsArray();	
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

void SCurrencyByTradeType::AddChieldAt(int32 Idx)
{
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_ArrayCurrecies->AsArray();	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	if (!(Idx > INDEX_NONE && Idx < (int32)childrenCount)) return;
	
	auto elementHandle = PropertyHandle_ArrayCurrecies->GetChildHandle(Idx);
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
				.OnClicked(this, &SCurrencyByTradeType::Event_RemoveCurrency)
			]
		]
	];
}

void SCurrencyByTradeType::Event_RemoveCurrency(int32 Idx)
{
	if (!PropertyHandle_ArrayCurrecies.IsValid()) return;
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_ArrayCurrecies->AsArray();
	if (Idx < 0) return;
		
	arrayPropertyHandle->DeleteItem(Idx);
	Widget_Currencies->RemoveSlot(Idx);
}


FReply SCurrencyByTradeType::SIGNATURE_BTN_VIEW()
{
	PropertyHandle_bIsExpended->SetValue((Widget_Expand.Get() && Widget_Expand.Get()->IsExpanded()));
	return FReply::Handled();
}

FReply SCurrencyByTradeType::SIGNATURE_BTN_ADD()
{
	if (!PropertyHandle_ArrayCurrecies.IsValid()) return FReply::Handled();

	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_ArrayCurrecies->AsArray();
	arrayPropertyHandle->AddItem();
	
	uint32 childrenCount = 0;
	arrayPropertyHandle->GetNumElements(childrenCount);
	AddChieldAt((childrenCount - 1));

	return FReply::Handled();
}

FReply SCurrencyByTradeType::SIGNATURE_BTN_CLEAR()
{
	if (!PropertyHandle_ArrayCurrecies.IsValid()) return FReply::Handled();
	
	TSharedPtr<IPropertyHandleArray> arrayPropertyHandle = PropertyHandle_ArrayCurrecies->AsArray();
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

TSharedRef<SWidget> SCurrencyByTradeType::OnGetKeyContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<SCurrencyByTradeType*>(this), &SCurrencyByTradeType::OnKeyComboChange, Idx));
		MenuBuilder.AddMenuEntry(FText::FromName(KeyValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void SCurrencyByTradeType::OnKeyComboChange(int32 Index)
{
	if (KeyValues.IsValidIndex(Index))
	{
		if (PropertyHandle_TradeType.IsValid())
		{
			PropertyHandle_TradeType->SetValue(KeyValues[Index]);
		}
	}
}

FText SCurrencyByTradeType::GetCurrentKeyDesc() const
{
	FName NameValue;
	PropertyHandle_TradeType->GetValue(NameValue);
	const int32 KeyIdx = KeyValues.IndexOfByKey(NameValue);
	return KeyValues.IsValidIndex(KeyIdx) ? FText::FromName(KeyValues[KeyIdx]) : FText::FromName(NameValue);
}