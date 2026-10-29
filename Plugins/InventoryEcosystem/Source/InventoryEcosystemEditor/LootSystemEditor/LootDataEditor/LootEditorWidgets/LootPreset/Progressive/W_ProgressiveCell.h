#pragma once

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_ArrayController.h"
#include "Widgets/SCompoundWidget.h"

class SW_AC_ProgressiveCellArray: public SW_ArrayController
{
public:
	virtual TSharedRef<SW_BaseArrayElement> CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor) override;
};

class SW_ProgressiveCell : public SW_BaseArrayElement
{
public:
	
	SLATE_BEGIN_ARGS(SW_ProgressiveCell)
			: _PropertyHandle_Array(nullptr),
			_PropertyHandle_Key(nullptr),
			_LootAssetEditorRef(nullptr),
			_ElementIdx(INDEX_NONE)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Array)
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Key)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_ARGUMENT(int32, ElementIdx)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:    
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	TSharedPtr<SVerticalBox> CurrentArrayWidget;
	TSharedPtr<SWidget> W_KeyInt;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_Array;
	TSharedPtr<IPropertyHandle> PropertyHandle_Key;
	TSharedPtr<IPropertyHandle> PropertyHandle_ProgressiveData;
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerateRules;
};
