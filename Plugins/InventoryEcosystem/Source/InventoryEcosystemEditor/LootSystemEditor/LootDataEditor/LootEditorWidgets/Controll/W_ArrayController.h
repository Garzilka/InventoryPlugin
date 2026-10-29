#pragma once

#include "W_BaseArrayElement.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/SCompoundWidget.h"

class SW_ArrayController: public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ArrayController)
		: _ArrayRef(nullptr),
		_PropertyHandle_OwnerStruct(nullptr),
		_DisplayName(FText::FromString("Array")),
		_LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>()),
		_EdgingColor(FLinearColor(0.f,0.f,0.f, 0.f)),
		_BackgroundColor(FLinearColor(2.f,2.f,2.f, 0.f)),
		_CustomWidget(nullptr)
	{}

	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, ArrayRef)
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_OwnerStruct)
	SLATE_ARGUMENT(FText, DisplayName)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_ARGUMENT(FLinearColor, EdgingColor)
	SLATE_ARGUMENT(FLinearColor, BackgroundColor)
	SLATE_ARGUMENT(TSharedPtr<SWidget>, CustomWidget)
		
SLATE_END_ARGS();
	
	void Construct(const FArguments& InArgs);	
	void RemoveAt(int32 Id);
	FReply RemoveAll();
	virtual TSharedRef<SW_BaseArrayElement> CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx,
		TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor);
	virtual TSharedRef<SWidget> CreateAdditional() {return SNew(SBox); };
protected:
	TSharedRef<SWidget> CreateArray();
	FReply AddButtonClicked();
	
	FText DisplayName;
	TSharedPtr<SWidget> CustomWidget;
	TSharedPtr<SVerticalBox> DisplayNamePlace;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<IPropertyHandle> PropertyHandle_OwnerStruct;
	TSharedPtr<SVerticalBox> VB_Array;
	TSharedPtr<IPropertyHandle> PropertyHandle_Array;
	TSharedPtr<SExpandableArea> CategoryExpand;
};
