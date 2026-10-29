#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

DECLARE_DELEGATE_OneParam(FClickerdAddListSignature, class SW_ListAddButton*)


class SW_ListAddButton  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ListAddButton)	{}
	SLATE_EVENT( FClickerdAddListSignature, OnClicked )
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_ListAddButton() {};
	void Construct(const FArguments& InArgs);
protected:
	FSlateColor GetColor() const;
	TSharedPtr<SButton> CategoryBTN;
	FReply Clicked();
	/** The delegate to execute when the button is clicked */
	FClickerdAddListSignature OnClicked;
	
};
