#include "W_AmountGenerateSettings.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"

void SW_AmountGenerateSettings::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	if (!PropertyHandle_Struct.IsValid()) return;

	PropertyHandle_Min = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountGenerateSettings, Min));
	PropertyHandle_Max = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountGenerateSettings, Max));
	PropertyHandle_TypeOfGenerate = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountGenerateSettings, TypeOfGenerate));
	PropertyHandle_TypeTypeCheckClass = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountGenerateSettings, TypeTypeCheckClass));

	
	ChildSlot
	[
		
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.75f, 0.f, 0.f))
		.Padding(3)
		.Content()
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0, 0, 0))
			.Padding(5.f)
			.Content()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.AutoWidth()
				[
					PropertyHandle_TypeOfGenerate->CreatePropertyValueWidget()
				]
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.AutoWidth()
				[
					PropertyHandle_Min->CreatePropertyValueWidget()
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					PropertyHandle_Max->CreatePropertyValueWidget()
				]
				+ SHorizontalBox::Slot()
				.Padding(2.f, 0.f)
				.AutoWidth()
				[
					PropertyHandle_TypeTypeCheckClass->CreatePropertyValueWidget()
				]
			]
		]
		
	];
}
