#pragma once

#include "Widgets/SCompoundWidget.h"
#include "IPropertyTypeCustomization.h"

class SCurrencyType : public SCompoundWidget
{

public:
	
	SLATE_BEGIN_ARGS(SCurrencyType)
		: _StructPropertyHandle(nullptr)
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(int32, RowID)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SCurrencyType() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);
protected:
	
	TSharedRef<SWidget> OnGetKeyContent() const;
	void OnKeyComboChange(int32 Index);
	FText GetCurrentKeyDesc() const;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;	
	TSharedPtr<IPropertyHandle> PropertyHandle_CurrencyType;
	
	TArray<FName> KeyValues;
};
