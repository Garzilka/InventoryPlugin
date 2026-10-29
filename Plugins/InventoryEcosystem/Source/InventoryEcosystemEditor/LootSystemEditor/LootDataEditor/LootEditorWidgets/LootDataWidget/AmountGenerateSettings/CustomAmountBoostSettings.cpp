// Copyright (c) 2022 GFA Games. All rights reserved.


#include "CustomAmountBoostSettings.h"

#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"


void SCustomAmountBoostSettings::Construct(const FArguments& InArgs)
{	
	StructPropertyHandle = InArgs._StructPropertyHandle;
	RowID = InArgs._RowID;
	OnClicked = InArgs._OnClicked;
	OnChangePosition = InArgs._OnChangePosition;
	
	if (!StructPropertyHandle.IsValid()) return;

	// LootTagPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountBoostSettings, LootTag));
	// WeightBonusType = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountBoostSettings, WeightBonusType));
	// MinPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountBoostSettings, Min));
	// MaxPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FAmountBoostSettings, Max));

	// and check them before using them
	check(LootTagPropertyHandle.IsValid() && WeightBonusType.IsValid() && MinPropertyHandle.IsValid() && MaxPropertyHandle.IsValid());

	
	// ChildSlot
	// [
	// 	SNew(SHorizontalBox)
	// 	+ SHorizontalBox::Slot()
	// 	.Padding(4.f, 0.f)
	// 	.AutoWidth()
	// 	.VAlign(VAlign_Center)
	// 	[
	// 		SNew(SVerticalBox)
	// 		+ SVerticalBox::Slot()
	// 		.AutoHeight()
	// 		[
	// 			SNew(SUpDownButton)
	// 			.bIsUp(true)
	// 			.OnClicked(this, &SCustomAmountBoostSettings::Event_UpButtonClicked)
	// 		]
	// 		+ SVerticalBox::Slot()
	// 		.AutoHeight()
	// 		[
	// 			SNew(SUpDownButton)
	// 			.bIsUp(false)
	// 			.OnClicked(this, &SCustomAmountBoostSettings::Event_DownButtonClicked)
	// 		]
	// 	]
	// 	+ SHorizontalBox::Slot()
	// 	.Padding(4.f, 0.f)
	// 	.AutoWidth()
	// 	.VAlign(VAlign_Center)
	// 	[
	// 		SNew(SCustomDeleteRowButton)
	// 		.OnClicked(this, &SCustomAmountBoostSettings::Event_Remove)
	// 	]
	// 	+ SHorizontalBox::Slot()
	// 	.Padding(3.f, 0.f)
	// 	.AutoWidth()
	// 	[
	// 		MakeTagContainer()
	// 	]
	// 	+ SHorizontalBox::Slot()
	// 	.Padding(3.f, 0.f)
	// 	.AutoWidth()
	// 	[
	// 		WeightBonusType->CreatePropertyValueWidget()
	// 	]
	// 	+ SHorizontalBox::Slot()
	// 	.Padding(3.f, 0.f)
	// 	.AutoWidth()
	// 	[
	// 		MinPropertyHandle->CreatePropertyValueWidget()
	// 	]
	// 	+ SHorizontalBox::Slot()
	// 	.AutoWidth()
	// 	[
	// 		MaxPropertyHandle->CreatePropertyValueWidget()
	// 	]
	// ];
}

FReply SCustomAmountBoostSettings::Event_Remove() const
{
	OnClicked.ExecuteIfBound(RowID);	
	return FReply::Handled();
}


FReply SCustomAmountBoostSettings::Event_UpButtonClicked() const
{
	OnChangePosition.ExecuteIfBound(true, RowID);	
	return FReply::Handled();
}

FReply SCustomAmountBoostSettings::Event_DownButtonClicked() const
{
	OnChangePosition.ExecuteIfBound(false, RowID);	
	return FReply::Handled();
}