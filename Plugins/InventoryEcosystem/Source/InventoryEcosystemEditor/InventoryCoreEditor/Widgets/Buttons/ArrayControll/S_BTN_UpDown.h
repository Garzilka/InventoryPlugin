// Copyright (c) 2022 GFA Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"


class SW_BTN_UpDown : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_BTN_UpDown)
		: _bIsUp(false)	{}

	SLATE_ARGUMENT(bool, bIsUp)
	SLATE_EVENT( FOnClicked, OnClicked )
		
SLATE_END_ARGS();
	INVENTORYCOREEDITOR_API void Construct(const FArguments& InArgs);
	void SetOnClicked(FOnClicked InOnClicked);
protected:
	bool bIsUp = false;
	const FSlateBrush* GetExpanderImage() const;
	FReply OnArrowClicked();
	FOnClicked OnClicked;
};
