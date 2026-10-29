#include "FCustomizationCurrencyType.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "UITrade"

TSharedRef<IPropertyTypeCustomization> FCustomizationCurrencyType::MakeInstance()
{
	return MakeShareable(new FCustomizationCurrencyType());
}

void FCustomizationCurrencyType::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FCustomizationCurrencyType::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!StructPropertyHandle->IsValidHandle()) return;
	
	WidgetStatePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyType, CurrencyType));
	
	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
	if (!TradeSettings) return;
	
	if (!WidgetStatePropertyHandle.IsValid()) return;

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

	StructBuilder.AddCustomRow(LOCTEXT("UITrade", "CurrencyType"))
		.NameContent()
		[
			WidgetStatePropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SComboButton)
				.OnGetMenuContent(this, &FCustomizationCurrencyType::OnGetKeyContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
						.Text(this, &FCustomizationCurrencyType::GetCurrentKeyDesc)
						.Font(IDetailLayoutBuilder::GetDetailFont())
				]
		];
	FName CurrentValue;
	WidgetStatePropertyHandle->GetValue(CurrentValue);
	if (CurrentValue == NAME_None)
	{
		WidgetStatePropertyHandle->SetValue(KeyValues[0]);
	}
}

TSharedRef<SWidget> FCustomizationCurrencyType::OnGetKeyContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<FCustomizationCurrencyType*>(this), &FCustomizationCurrencyType::OnKeyComboChange, Idx));
		MenuBuilder.AddMenuEntry(FText::FromName(KeyValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void FCustomizationCurrencyType::OnKeyComboChange(int32 Index)
{
	if (KeyValues.IsValidIndex(Index))
	{
		if (WidgetStatePropertyHandle.IsValid())
		{
			WidgetStatePropertyHandle->SetValue(KeyValues[Index]);
		}
	}
}

FText FCustomizationCurrencyType::GetCurrentKeyDesc() const
{
	FName NameValue;
	WidgetStatePropertyHandle->GetValue(NameValue);
	const int32 KeyIdx = KeyValues.IndexOfByKey(NameValue);
	return KeyValues.IsValidIndex(KeyIdx) ? FText::FromName(KeyValues[KeyIdx]) : FText::FromName(NameValue);
}

#undef LOCTEXT_NAMESPACE