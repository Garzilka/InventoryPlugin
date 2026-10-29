// Copyright (c) 2022 GFA Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

class SW_BTN_Add : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_BTN_Add)	{}
	SLATE_EVENT( FOnClicked, OnClicked )
		
SLATE_END_ARGS();
	
	INVENTORYCOREEDITOR_API void Construct(const FArguments& InArgs);
	void SetOnClicked(FOnClicked InOnClicked);
protected:
	FReply Event_Clicked();
	FOnClicked OnClicked;
};
