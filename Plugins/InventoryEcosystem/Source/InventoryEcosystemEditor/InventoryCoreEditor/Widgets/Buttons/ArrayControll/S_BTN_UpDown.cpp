// Copyright (c) 2022 GFA Games. All rights reserved.


#include "S_BTN_UpDown.h"
#include "EditorStyleSet.h"


void SW_BTN_UpDown::Construct(const FArguments& InArgs)
{
	bIsUp = InArgs._bIsUp;
	OnClicked = InArgs._OnClicked;
	ChildSlot.Padding( FMargin( 0.f))
	[
		SNew(SButton)
		.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Visibility( EVisibility::Visible )
		.ClickMethod( EButtonClickMethod::MouseDown )
		.OnClicked( this, &SW_BTN_UpDown::OnArrowClicked )
		.ContentPadding(0.f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.IsFocusable( false )
		[
			SNew(SImage)
			.Image( this, &SW_BTN_UpDown::GetExpanderImage )
			.ColorAndOpacity( FSlateColor::UseForeground() )
		]
	];
}

void SW_BTN_UpDown::SetOnClicked(FOnClicked InOnClicked)
{
	OnClicked = InOnClicked;
}


FReply SW_BTN_UpDown::OnArrowClicked()
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

const FSlateBrush* SW_BTN_UpDown::GetExpanderImage() const
{
	return (bIsUp ? FAppStyle::GetBrush("SurfaceDetails.PanVNegative") : FAppStyle::GetBrush("SurfaceDetails.PanVPositive")); 
}