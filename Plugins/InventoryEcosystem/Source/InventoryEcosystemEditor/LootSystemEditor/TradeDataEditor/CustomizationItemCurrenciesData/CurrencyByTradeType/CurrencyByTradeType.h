#pragma once

#include "Widgets/SCompoundWidget.h"
#include "IPropertyTypeCustomization.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Tools/AdvancedVerticalBox.h"

class SCurrencyByTradeType : public SCompoundWidget
{

public:
	
	SLATE_BEGIN_ARGS(SCurrencyByTradeType)
		: _StructPropertyHandle(nullptr)
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(const ISlateStyle*, StyleSet)
	/** Called when the button is clicked */
	SLATE_EVENT( FOnClicked, OnClicked )
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SCurrencyByTradeType() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);
protected:
	TSharedRef<SWidget> MakeArrayWidget();
	void AddChieldAt(int32 Idx);
	
	TSharedRef<SWidget> OnGetKeyContent() const;
	void OnKeyComboChange(int32 Index);
	FText GetCurrentKeyDesc() const;
	
	FReply SIGNATURE_BTN_VIEW();
	FReply SIGNATURE_BTN_ADD();
	FReply SIGNATURE_BTN_CLEAR();

	void Event_RemoveCurrency(int32 Idx);
	
	TSharedPtr<SExpandableArea> Widget_Expand;
	TSharedPtr<SAdvancedVerticalBox> Widget_Currencies;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_TradeType;
	TSharedPtr<IPropertyHandle> PropertyHandle_bIsExpended;
	TSharedPtr<IPropertyHandle> PropertyHandle_ArrayCurrecies;
	
	TArray<FName> KeyValues;
};
