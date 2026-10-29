#pragma once

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_BaseArrayElement.h"
#include "Widgets/SCompoundWidget.h"

class SW_ProgressiveCategory : public SW_BaseArrayElement
{
public:
	
	SLATE_BEGIN_ARGS(SW_ProgressiveCategory)
			:   _PropertyHandle_Struct(nullptr),
				_LootAssetEditorRef(nullptr),
				_ElementIdx(INDEX_NONE)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_ARGUMENT(int32, ElementIdx)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_ProgressiveCategory() {};
	void Construct(const FArguments& InArgs);
	FText GetText() const;
	EVisibility GetVisibilityRandom() const;
	EVisibility GetVisibilityProbable() const;
protected:
	FReply SIGNATURE_BTN_REMOVE();
    
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	TSharedPtr<SVerticalBox> CurrentArrayWidget;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_CategoryName;
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerationType;
	TSharedPtr<IPropertyHandle> PropertyHandle_RandomGeneration;
	TSharedPtr<IPropertyHandle> PropertyHandle_ProbableGeneration;
};
