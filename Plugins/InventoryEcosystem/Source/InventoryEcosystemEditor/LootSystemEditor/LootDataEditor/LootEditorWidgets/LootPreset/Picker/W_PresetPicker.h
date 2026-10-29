#pragma once

#include "W_LootPicker.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "RowPickerCategoryList/W_RowPickerObjectBTN.h"
#include "Widgets/SCompoundWidget.h"

class SW_PresetPicker  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_PresetPicker)
		:	_PropertyHandle_Struct(nullptr),
			_LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_PresetPicker() {};
	
	LOOTSYSTEMEDITOR_API FText GetText() const;
	FText OnGetText() const;
	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> OnGetKeyContent() const;
	FText GetCurrentKeyDesc() const;
	void EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID);
protected:
	FText LootSpawner_Name;
	FText Path;
	
	TSharedPtr<SVerticalBox> CB_RowPicker;
	TSharedPtr<SMyComboMenu> CM_Menu;
	TSharedPtr<SComboButton> CB_Selector;
	
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_PresetGUID;
};
