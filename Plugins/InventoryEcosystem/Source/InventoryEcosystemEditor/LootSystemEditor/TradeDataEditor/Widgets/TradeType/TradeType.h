#pragma once
#include "PropertyHandle.h"
#include "Widgets/SCompoundWidget.h"

class STradeType : public SCompoundWidget
{

public:
	
	SLATE_BEGIN_ARGS(STradeType)
		: _StructPropertyHandle(nullptr)
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(int32, RowID)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API STradeType() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);
protected:
	
	TSharedRef<SWidget> OnGetKeyContent() const;
	void OnKeyComboChange(int32 Index);
	FText GetCurrentKeyDesc() const;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;	
	TSharedPtr<IPropertyHandle> PropertyHandle_CurrencyType;
	
	TArray<FName> KeyValues;
};
