// Copyright (c) 2022 GFA Games. All rights reserved.


#include "S_BTN_DeleteSingle.h"
#include "EditorStyleSet.h"

void SW_BTN_DeleteSingle::Construct(const FArguments& InArgs)
{
	OnClicked = InArgs._OnClicked;
	ChildSlot.Padding( FMargin( 0.f))
	[
		SNew(SButton)
		.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Visibility( EVisibility::Visible )
		.ClickMethod( EButtonClickMethod::MouseDown )
		.OnClicked( this, &SW_BTN_DeleteSingle::Event_Clicked )
		.ContentPadding(0.f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.IsFocusable( false )
		[
			SNew(SImage)
			.Image( FAppStyle::GetBrush("Icons.Delete") )
			.ColorAndOpacity( FSlateColor::UseForeground() )
		]
	];
}

void SW_BTN_DeleteSingle::SetOnClicked(FOnClicked InOnClicked)
{
	OnClicked = InOnClicked;
}

FReply SW_BTN_DeleteSingle::Event_Clicked()
{
	// Recurse the expansion if "shift" is being pressed
	if (OnClicked.IsBound())
	{
		FReply Reply = OnClicked.Execute();
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