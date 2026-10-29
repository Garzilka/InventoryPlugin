#pragma once
#include "PropertyHandle.h"
#include "Widgets/SCompoundWidget.h"

class SW_StandardizedRow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SW_StandardizedRow)
		: _PropertyHandle(nullptr),
		_CustomWidget(nullptr),
		_DisplayName(FText::FromString("Array"))
		
	{}

	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)
	SLATE_ARGUMENT(TSharedPtr<SWidget>, CustomWidget)
	SLATE_ARGUMENT(FText, DisplayName)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:
	
	FText DisplayName;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SWidget> CustomWidget;
	TSharedPtr<SVerticalBox> Place;
};
