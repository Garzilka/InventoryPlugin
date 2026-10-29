// Copyright (c) 2022 GFA Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SGameplayTagWidget.h"



DECLARE_DELEGATE_OneParam(FOnRemoveWeightSignature, int32)
DECLARE_DELEGATE_TwoParams(FOnUpDownAmountBoostSignature, bool, int32)

class SCustomAmountBoostSettings : public SCompoundWidget
{

public:

	DECLARE_DELEGATE_OneParam(FOnTagChanged, const FGameplayTag /*Tag*/)

	SLATE_BEGIN_ARGS(SCustomAmountBoostSettings)
		 : _StructPropertyHandle(nullptr),
		  _RowID(-1)
	{}

	// If set, the tag is read from the property, and the property is changed when tag is edited. 
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, StructPropertyHandle)
	SLATE_ARGUMENT(int32, RowID)
	/** Called when the button is clicked */
	SLATE_EVENT( FOnRemoveWeightSignature, OnClicked )
	SLATE_EVENT( FOnUpDownAmountBoostSignature, OnChangePosition )

		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SCustomAmountBoostSettings() {};
	
	LOOTSYSTEMEDITOR_API void Construct(const FArguments& InArgs);

private:
	int32 RowID = -1;
	/** The delegate to execute when the button is clicked */
	FOnRemoveWeightSignature OnClicked;
	FOnUpDownAmountBoostSignature OnChangePosition;
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> WeightBonusType;
	TSharedPtr<IPropertyHandle> MinPropertyHandle;
	TSharedPtr<IPropertyHandle> MaxPropertyHandle;
	TSharedPtr<IPropertyHandle> LootTagPropertyHandle;
	
	FReply Event_Remove() const;
	FReply Event_UpButtonClicked() const;
	FReply Event_DownButtonClicked() const;
};
