#pragma once
#include "Widgets/SCompoundWidget.h"


DECLARE_DELEGATE_OneParam(FRemoveElementFromArray, int32)

class SW_BaseArrayElement : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_BaseArrayElement)
		: _ElementIdx(INDEX_NONE)
	{}
	SLATE_ARGUMENT(int32, ElementIdx)
	SLATE_EVENT( FRemoveElementFromArray, OnClicked )
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);

	FRemoveElementFromArray OnClicked;
	int32 ElementIdx;
};