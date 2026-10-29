#pragma once

#include "Widgets/SCompoundWidget.h"
#include "IPropertyTypeCustomization.h"

DECLARE_DELEGATE_OneParam(FOnCurrencyValueSignature, int32)

class SCurrencyValue : public SCompoundWidget
{
	//SLATE_DECLARE_WIDGET_API(SGameplayTagCombo, SCompoundWidget, SPATIALINVENTORYEDITOR_API)

public:
	
	SLATE_BEGIN_ARGS(SCurrencyValue)
		: _StructPropertyHandle(nullptr),
		  _RowID(-1)
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(int32, RowID)

	SLATE_EVENT( FOnCurrencyValueSignature, OnClicked )
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SCurrencyValue() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);
protected:
	int32 RowID = -1;
	FOnCurrencyValueSignature OnClicked;
	FReply SIGNATURE_BTN_REMOVE() const;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_CurrencyType;
	TSharedPtr<IPropertyHandle> PropertyHandle_CurrencyValue;
};