#pragma once
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Tools.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"

class SW_LootMetaData : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SW_LootMetaData)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditor(nullptr)
		
	{}

	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditor)
	SLATE_ARGUMENT(FName, LootObjectGUID)
	SLATE_ARGUMENT(ECategorySelector, ObjectType)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:
	TSharedPtr<SEditableTextBox> SETB_Category;
	TSharedPtr<SEditableTextBox> SETB_Name;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	ECategorySelector ObjectType;
	FName LootObjectGUID;
	
	FText LootDataObject_CurrentName;
	FText LootDataObject_OldName;
	
	FName LootDataObject_CurrentCategory;
	FName LootDataObject_OldCategory;
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_Category;
	TSharedPtr<IPropertyHandle> PropertyHandle_Name;
	void CategoryChanged();
	void NameChanged();
};
