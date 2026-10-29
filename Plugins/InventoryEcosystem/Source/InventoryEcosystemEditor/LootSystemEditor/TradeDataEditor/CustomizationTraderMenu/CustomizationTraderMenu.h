#pragma once
#include "IPropertyTypeCustomization.h"
#include "Components/VerticalBox.h"

class FCustomizationTraderMenu : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// BEGIN IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	void AddChieldAt(int32 Idx);
	// END IPropertyTypeCustomization interface

	TSharedRef<SWidget> MakeArrayWidget();
	FText GetCurrentKeyDesc() const;
	
	FReply SIGNATURE_BTN_VIEW();
	FReply SIGNATURE_BTN_ADD();
	FReply SIGNATURE_BTN_CLEAR();
	
	void EVENT_REMOVE_SCORE(int32 Idx);
	EVisibility GetVisibilityArray() const;

	TSharedPtr<SVerticalBox> Widget_Currencies;
	TSharedPtr<SExpandableArea> Widget_Expand;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_bIsExpended;
	TSharedPtr<IPropertyHandle> PropertyHandle_TradeType;
	TSharedPtr<IPropertyHandle> PropertyHandle_Array;
};
