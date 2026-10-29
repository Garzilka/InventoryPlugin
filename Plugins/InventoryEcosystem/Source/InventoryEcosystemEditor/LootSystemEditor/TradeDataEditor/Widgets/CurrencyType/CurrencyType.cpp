#include "CurrencyType.h"

#include "DetailLayoutBuilder.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"


void SCurrencyType::Construct(const FArguments& InArgs)
{	
	PropertyHandle_Struct = InArgs._StructPropertyHandle;
	if (!PropertyHandle_Struct.IsValid()) return;

	PropertyHandle_CurrencyType = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurrencyType, CurrencyType));

	const ULootSystemSetting* LootSystemSetting = ULootSystemSetting::Get();
	auto TradeSettings = LootSystemSetting->TradeSettings.LoadSynchronous();
	if (!TradeSettings) return;
	
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
	
	ChildSlot
	[
		SNew(SBox)
			.WidthOverride(120.f)
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &SCurrencyType::OnGetKeyContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
						.Text(this, &SCurrencyType::GetCurrentKeyDesc)
						.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			]
	];
	
	FName CurrentValue;
	PropertyHandle_CurrencyType->GetValue(CurrentValue);
	if (CurrentValue == NAME_None)
	{
		PropertyHandle_CurrencyType->SetValue(KeyValues[0]);
	}
}


TSharedRef<SWidget> SCurrencyType::OnGetKeyContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<SCurrencyType*>(this), &SCurrencyType::OnKeyComboChange, Idx));
		MenuBuilder.AddMenuEntry(FText::FromName(KeyValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void SCurrencyType::OnKeyComboChange(int32 Index)
{
	if (KeyValues.IsValidIndex(Index))
	{
		if (PropertyHandle_CurrencyType.IsValid())
		{
			PropertyHandle_CurrencyType->SetValue(KeyValues[Index]);
		}
	}
}

FText SCurrencyType::GetCurrentKeyDesc() const
{
	FName NameValue;
	PropertyHandle_CurrencyType->GetValue(NameValue);
	const int32 KeyIdx = KeyValues.IndexOfByKey(NameValue);
	return KeyValues.IsValidIndex(KeyIdx) ? FText::FromName(KeyValues[KeyIdx]) : FText::FromName(NameValue);
}