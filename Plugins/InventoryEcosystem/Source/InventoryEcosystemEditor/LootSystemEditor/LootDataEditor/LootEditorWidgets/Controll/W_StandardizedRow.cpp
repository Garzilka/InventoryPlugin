#include "W_StandardizedRow.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

void SW_StandardizedRow::Construct(const FArguments& InArgs)
{
	DisplayName = InArgs._DisplayName;
	PropertyHandle = InArgs._PropertyHandle;
	CustomWidget = InArgs._CustomWidget;
	
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(120.f)
			[
				SNew(STextBlock)
				.Text(DisplayName)
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(Place, SVerticalBox)
		]
	];
	
	Place->ClearChildren();
	if (PropertyHandle && PropertyHandle.IsValid())
	{
		Place->AddSlot()
		[
			PropertyHandle->CreatePropertyValueWidget()
		];
		return;
	}
	
	if (CustomWidget == nullptr) return;
	
	Place->AddSlot()
	[
		CustomWidget.ToSharedRef()
	];
}
