#pragma once

#include "CoreMinimal.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SCompoundWidget.h"

class SCategoryBTN : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SCategoryBTN)
		: _CategoryName(FName("Empty"))
	{}
	SLATE_ARGUMENT(FName, CategoryName)
		
SLATE_END_ARGS();
	FText GetText() const;
	void Construct(const FArguments& InArgs);
protected:
	FName CategoryName;
};
