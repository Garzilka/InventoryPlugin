#include "W_RowPickerObjectBTN.h"

#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Application/SlateApplicationBase.h"
#include "Widgets/Accessibility/SlateAccessibleMessageHandler.h"

FText SW_RowPickerObjectBTN::GetText() const
{
	return LootSpawner_Name;
}

void SW_RowPickerObjectBTN::Construct(const FArguments& InArgs)
{
	LootObjectGUID = InArgs._LootObjectGUID;
	// LootAssetEditor = InArgs._LootAssetEditorRef;
	IsLoot = InArgs._IsLoot;

	if (IsLoot)
	{
		if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootData().Find(LootObjectGUID))
		{
			LootSpawner_Name = TextData->MetaData.Object_Name;
		}
	}
	else
	{
		if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootPresets().Find(LootObjectGUID))
		{
			LootSpawner_Name = TextData->MetaData.Object_Name;
		}
	}
	OnClicked = InArgs._OnClicked;
	
	this->ChildSlot
	.Padding( FMargin(0) )
	[
		SAssignNew(CategoryBTN, SButton)
		.ButtonStyle( FCoreStyle::Get(), "NoBorder")
		.ClickMethod( EButtonClickMethod::MouseDown )
		.OnClicked( this, &SW_RowPickerObjectBTN::Clicked )
		.ContentPadding(0.f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.IsFocusable( false )
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(this, &SW_RowPickerObjectBTN::GetColor)
			[
				SNew(SBox)
				.Padding(1.f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text( this, &SW_RowPickerObjectBTN::GetText )
				]
			]
		]
	];
}

void SW_RowPickerObjectBTN::SetSelected(bool InSelected)
{
	Selected = InSelected;
}

FSlateColor SW_RowPickerObjectBTN::GetColor() const
{
	return ((CategoryBTN.Get() && CategoryBTN.Get()->IsHovered()) ? FLinearColor(0.1f, 0.1f, 0.1f) : FLinearColor(0.5f, 0.5f, 0.5f));
}

FReply SW_RowPickerObjectBTN::Clicked()
{
	if (OnClicked.IsBound())
	{
		OnClicked.Execute(this);
		
		FReply Reply = FReply::Handled();
#if WITH_ACCESSIBILITY
		FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(FSlateAccessibleMessageHandler::FSlateWidgetAccessibleEventArgs(AsShared(), EAccessibleEvent::Activate));
#endif
		return Reply;
	}
	else
	{
		return FReply::Handled();
	}
}
