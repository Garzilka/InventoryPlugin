#include "S_BTN_ClearAll.h"


void SW_BTN_ClearAll::SetOnClicked(FOnClicked InOnClicked)
{
	OnClicked = InOnClicked;
}

FReply SW_BTN_ClearAll::OnArrowClicked()
{
	// return OnClicked.ExecuteIfBound(nullptr);
	return FReply::Handled();
}