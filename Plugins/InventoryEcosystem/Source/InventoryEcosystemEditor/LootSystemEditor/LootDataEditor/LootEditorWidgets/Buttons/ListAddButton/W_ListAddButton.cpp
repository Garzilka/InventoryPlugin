#include "W_ListAddButton.h"

#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Application/SlateApplicationBase.h"
#include "Widgets/Accessibility/SlateAccessibleMessageHandler.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"


void SW_ListAddButton::Construct(const FArguments& InArgs)
{
	OnClicked = InArgs._OnClicked;
	
	this->ChildSlot
	.Padding( FMargin(0) )
	[
		
		SNew(SBox)
		.HeightOverride(25.f)
		.MinDesiredWidth(100.f)
		[
			
			SAssignNew(CategoryBTN, SButton)
			.ButtonStyle( FCoreStyle::Get(), "NoBorder")
			.ClickMethod( EButtonClickMethod::MouseDown )
			.OnClicked( this, &SW_ListAddButton::Clicked )
			.ContentPadding(0.f)
			.ForegroundColor( FSlateColor::UseForeground() )
			.IsFocusable( false )
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.0f, 0.2f, 0.0f))
				[
					SNew(SBorder)
					.Padding(3.f)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.BorderBackgroundColor(this, &SW_ListAddButton::GetColor)
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("+"))
						]
					]
				]
			]
		]
	];
}


FSlateColor SW_ListAddButton::GetColor() const
{
	return ((CategoryBTN.Get() && CategoryBTN.Get()->IsHovered()) ? FLinearColor(0.0f, 0.4f, 0.0f) : FLinearColor(0.0f, 0.3f, 0.0f));
}

FReply SW_ListAddButton::Clicked()
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
