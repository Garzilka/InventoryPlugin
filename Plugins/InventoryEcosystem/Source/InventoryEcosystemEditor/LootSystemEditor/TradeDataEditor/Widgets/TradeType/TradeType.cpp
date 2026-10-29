#include "TradeType.h"



#include "DetailLayoutBuilder.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"


void STradeType::Construct(const FArguments& InArgs)
{	
	PropertyHandle_Struct = InArgs._StructPropertyHandle;
	if (!PropertyHandle_Struct.IsValid()) return;

	PropertyHandle_CurrencyType = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTradeType, TradeType));

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
			.WidthOverride(120.f)
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &STradeType::OnGetKeyContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
						.Text(this, &STradeType::GetCurrentKeyDesc)
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


TSharedRef<SWidget> STradeType::OnGetKeyContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 Idx = 0; Idx < KeyValues.Num(); Idx++)
	{
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<STradeType*>(this), &STradeType::OnKeyComboChange, Idx));
		MenuBuilder.AddMenuEntry(FText::FromName(KeyValues[Idx]), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void STradeType::OnKeyComboChange(int32 Index)
{
	if (KeyValues.IsValidIndex(Index))
	{
		if (PropertyHandle_CurrencyType.IsValid())
		{
			PropertyHandle_CurrencyType->SetValue(KeyValues[Index]);
		}
	}
}

FText STradeType::GetCurrentKeyDesc() const
{
	FName NameValue;
	PropertyHandle_CurrencyType->GetValue(NameValue);
	const int32 KeyIdx = KeyValues.IndexOfByKey(NameValue);
	return KeyValues.IsValidIndex(KeyIdx) ? FText::FromName(KeyValues[KeyIdx]) : FText::FromName(NameValue);
}