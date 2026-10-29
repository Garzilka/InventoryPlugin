#pragma once

#include "W_RowPickerObjectBTN.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Tools.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FSelectedRowPickerLootObjectCallbackSignature, class SW_RowPickerObjectBTN*)

class SW_RowPickerCategoryList  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_RowPickerCategoryList)
		: _CategoryData(FCategoryObjectData()),
		  _CurrentPath(NAME_None),
		  _CurrentRoot(NAME_None),
		  _IsLoot(true),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(FCategoryObjectData, CategoryData)
	SLATE_ARGUMENT(FName, CurrentPath)
	SLATE_ARGUMENT(FName, CurrentRoot)
	SLATE_ARGUMENT(bool, IsLoot)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_EVENT( FSelectedRowPickerLootObjectCallbackSignature, OnClicked )
		
SLATE_END_ARGS();
	
	void Construct(const FArguments& InArgs);
	FText GetText() const;
protected:
	void CheckChangeObject(FName ObjectGUID, FName OldCategory, FName NewCategory);
	FCategoryObjectData CategoryData;
	FName CurrentPath;
	FName CurrentRoot;
	bool IsLoot = true;
	TSharedPtr<SVerticalBox> CurrentLootObjects;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	void EventSelectedButton(SW_RowPickerObjectBTN* ObjectGUID);
	void EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID);

	TSharedRef<SWidget> CreateChildren(FCategoryObjectData Category, int32 Idx);
	TSharedRef<SWidget> CreateArray(TArray<FName> LootSettings);
	
	FSelectedRowPickerLootObjectCallbackSignature OnClicked;
	
};
