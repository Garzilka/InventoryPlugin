#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"


class SW_BTN_ClearAll : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_BTN_ClearAll)	{}
	SLATE_EVENT( FOnClicked, OnClicked )
		
SLATE_END_ARGS();
	INVENTORYCOREEDITOR_API void Construct(const FArguments& InArgs)
	{
		OnClicked = InArgs._OnClicked;
		ChildSlot.Padding(FMargin(0.f))
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.4f, 0.4f, 0.4f))
				[
					SAssignNew(ExpanderArrow, SButton)
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Visibility( EVisibility::Visible )
					.ClickMethod( EButtonClickMethod::MouseDown )
					.OnClicked( this, &SW_BTN_ClearAll::OnArrowClicked )
					.ContentPadding(0.f)
					.ForegroundColor( FSlateColor::UseForeground() )
					.IsFocusable( false )
					[
						SNew(SImage)
						.Image( FAppStyle::GetBrush("Icons.Delete") )
						.ColorAndOpacity( FSlateColor::UseForeground() )
					]
				]
			]
			
		];
	}
	
	void SetOnClicked(FOnClicked InOnClicked);
protected:
	TSharedPtr<SButton> ExpanderArrow;
	FReply OnArrowClicked();
	FOnClicked OnClicked;
};
