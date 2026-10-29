#pragma once
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/Input/SEditableTextBox.h"

class SW_LootPresetMainWidget  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_LootPresetMainWidget)
		: _LootObjectGUID(FName("")),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(FName, LootObjectGUID)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_LootPresetMainWidget() {};
	void Construct(const FArguments& InArgs);

	void Init(FName NewObjectGUID);
	void SetDefault();
protected:
	void UpdateHandleMap();
	void GenerateLootData();
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SVerticalBox> VB_LootData;
	TSharedPtr<SVerticalBox> B_MetaPlace;
	TSharedPtr<SExpandableArea> CategoryExpand;
	FName LootObjectGUID;
	FText LootDataObject_Name;
	FName LootDataObject_Category;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_HandleMap;
	TSharedPtr<IPropertyHandle> PropertyHandle_PresetStruct;
	TSharedPtr<IPropertyHandle> PropertyHandle_MetaData;
};
