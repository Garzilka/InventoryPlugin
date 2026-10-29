#include "CategoryBTN.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

FText SCategoryBTN::GetText() const
{
	return FText::FromName(CategoryName);
}

void SCategoryBTN::Construct(const FArguments& InArgs)
{
	CategoryName = InArgs._CategoryName;
	
	this->ChildSlot
	.Padding( FMargin(0) )
	[
		SNew(SBox)
		.HeightOverride(25.f)
		.MinDesiredWidth(100.f)
		[
			SNew(STextBlock)
			.Text( this, &SCategoryBTN::GetText )
		]
	];
}