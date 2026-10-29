#pragma once

class FCustomizationTradeType : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// BEGIN IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// END IPropertyTypeCustomization interface


	TSharedRef<SWidget> OnGetKeyContent() const;
	void OnKeyComboChange(int32 Index);
	FText GetCurrentKeyDesc() const;

	TArray<FName> KeyValues;

	TSharedPtr<IPropertyHandle> WidgetStatePropertyHandle;
};