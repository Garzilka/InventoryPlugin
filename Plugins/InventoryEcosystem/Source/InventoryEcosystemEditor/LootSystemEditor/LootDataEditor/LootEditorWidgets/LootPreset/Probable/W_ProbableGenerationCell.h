#pragma once

#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Internationalization/Text.h"

DECLARE_DELEGATE_OneParam(FRemoveProbableCellSignature, int32)

class SW_ProbableGenerationCell : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ProbableGenerationCell)
		: _CategoryName(FName("Empty")),
		 _PropertyHandle_Struct(nullptr),
		 _Cell_ID(INDEX_NONE),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(const ISlateStyle*, StyleSet)
	SLATE_ARGUMENT(FName, CategoryName)
	SLATE_ARGUMENT(int32, Cell_ID)
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_EVENT( FRemoveProbableCellSignature, OnClicked )
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:
	EVisibility GetVisibilityGenerateClass() const;
	EVisibility GetVisibilityItemClass() const;
	EVisibility GetVisibilityAmount() const;
	EVisibility GetVisibilityRange() const;
	FReply SIGNATURE_BTN_REMOVE();

	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	FRemoveProbableCellSignature OnClicked;
	int32 Cell_ID = INDEX_NONE;

	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;	
	TSharedPtr<IPropertyHandle> PropertyHandle_Percent;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_ItemContext;	
	TSharedPtr<IPropertyHandle> PropertyHandle_bUseCustomLogic;
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerateClass;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_ItemData;
	TSharedPtr<IPropertyHandle> PropertyHandle_ItemClass;
	TSharedPtr<IPropertyHandle> PropertyHandle_bIsRange;
	TSharedPtr<IPropertyHandle> PropertyHandle_Amount;
	TSharedPtr<IPropertyHandle> PropertyHandle_Range_X;
	TSharedPtr<IPropertyHandle> PropertyHandle_Range_Y;
	
};
