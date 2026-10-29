#pragma once

#include "Widgets/SCompoundWidget.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_BaseArrayElement.h"

class SW_RandomGenerationCell : public SW_BaseArrayElement
{
public:
	
	SLATE_BEGIN_ARGS(SW_RandomGenerationCell)
		: _CategoryName(FName("Empty")),
		 _PropertyHandle_Struct(nullptr),
		 _ElementIdx(INDEX_NONE),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(FName, CategoryName)
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(int32, ElementIdx)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:
	EVisibility GetVisibilityGenerateClass() const;
	EVisibility GetVisibilityItemClass() const;
	EVisibility GetVisibilityAmount() const;
	EVisibility GetVisibilityRange() const;
	FReply SIGNATURE_BTN_REMOVE();
	
	const FSlateBrush* GetImage() const;

	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	int32 Cell_ID;

	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;	
	TSharedPtr<IPropertyHandle> PropertyHandle_Weight;
	
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
