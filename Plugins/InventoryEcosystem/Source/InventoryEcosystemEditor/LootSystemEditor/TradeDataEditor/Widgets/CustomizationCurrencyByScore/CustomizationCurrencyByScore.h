#pragma once

#include "Widgets/SCompoundWidget.h"
#include "IPropertyTypeCustomization.h"
#include "Widgets/Layout/SExpandableArea.h"

class SVerticalBox;
DECLARE_DELEGATE_OneParam(FOnCurrencyValueByScoreSignature, int32)

class SCustomizationCurrencyByScore : public SCompoundWidget
{

public:
	
	SLATE_BEGIN_ARGS(SCustomizationCurrencyByScore)
		: _StructPropertyHandle(nullptr),
		  _RowID(-1)
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(int32, RowID)

	SLATE_EVENT( FOnCurrencyValueByScoreSignature, OnClicked )
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SCustomizationCurrencyByScore() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);
protected:
	int32 RowID = -1;
	FOnCurrencyValueByScoreSignature OnClicked;
	
	FReply SIGNATURE_BTN_VIEW() const;
	FReply SIGNATURE_BTN_ADD();
	FReply SIGNATURE_BTN_CLEAR();
	FReply SIGNATURE_BTN_REMOVE();
	
	void Event_RemoveCurrency(int32 Idx);
	
	TSharedRef<SWidget> MakeArrayWidget();
	void AddChieldAt(int32 Idx);
	TSharedPtr<SVerticalBox> Widget_Currencies;
	TSharedPtr<SExpandableArea> Widget_Expand;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;	
	TSharedPtr<IPropertyHandle> PropertyHandle_Score;
	TSharedPtr<IPropertyHandle> PropertyHandle_Array;
	TSharedPtr<IPropertyHandle> PropertyHandle_bIsExpended;
};
