#include "FCustomizationCurrencyValue.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "Widgets/Input/SComboButton.h"


#define LOCTEXT_NAMESPACE "UITrade"

TSharedRef<IPropertyTypeCustomization> FCustomizationCurrencyValue::MakeInstance()
{
	return MakeShareable(new FCustomizationCurrencyValue());
}

void FCustomizationCurrencyValue::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FCustomizationCurrencyValue::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!StructPropertyHandle->IsValidHandle()) return;
	
	TypePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValue, CurrencyType));
	ValuePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyValue, Value));

		
	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
	if (!TradeSettings) return;

	if (!TypePropertyHandle.IsValid()) return;
	if (!ValuePropertyHandle.IsValid()) return;

	if (TradeSettings->CurrencySettings.Num() > 0)
	{
		KeyValues.Reset();
		for (auto LCurrencyType : TradeSettings->CurrencySettings)
		{
			KeyValues.Add(LCurrencyType.Key);
		}
	}
	else
	{
		KeyValues.Reset();
		KeyValues.Add(TEXT("None"));
	}

	StructBuilder.AddCustomRow(LOCTEXT("UITrade", "TradeValue"))
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &FCustomizationCurrencyValue::OnGetKeyContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
						.Text(this, &FCustomizationCurrencyValue::GetCurrentKeyDesc)
						.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			]
			+ SHorizontalBox::Slot()
			[
				ValuePropertyHandle->CreatePropertyValueWidget()
			]
		];
	FName CurrentValue;
	TypePropertyHandle->GetValue(CurrentValue);
	if (CurrentValue == NAME_None)
	{
		TypePropertyHandle->SetValue(KeyValues[0]);
	}
}

TSharedRef<SWidget> FCustomizationCurrencyValue::OnGetKeyContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<FCustomizationCurrencyValue*>(this), &FCustomizationCurrencyValue::OnKeyComboChange, Idx));
		MenuBuilder.AddMenuEntry(FText::FromName(KeyValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void FCustomizationCurrencyValue::OnKeyComboChange(int32 Index)
{
	if (KeyValues.IsValidIndex(Index))
	{
		if (TypePropertyHandle.IsValid())
		{
			TypePropertyHandle->SetValue(KeyValues[Index]);
		}
	}
}

FText FCustomizationCurrencyValue::GetCurrentKeyDesc() const
{
	FName NameValue;
	TypePropertyHandle->GetValue(NameValue);
	const int32 KeyIdx = KeyValues.IndexOfByKey(NameValue);
	return KeyValues.IsValidIndex(KeyIdx) ? FText::FromName(KeyValues[KeyIdx]) : FText::FromName(NameValue);
}

#undef LOCTEXT_NAMESPACE