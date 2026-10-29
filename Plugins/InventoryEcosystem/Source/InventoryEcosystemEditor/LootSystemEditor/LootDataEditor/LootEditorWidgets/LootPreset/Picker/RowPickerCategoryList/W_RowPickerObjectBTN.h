#pragma once

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FSelectedRowPickerLootObjectSignature, class SW_RowPickerObjectBTN*)

class SW_RowPickerObjectBTN  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_RowPickerObjectBTN)
		: _LootObjectGUID(FName("")),
		  _IsLoot(true),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(FName, LootObjectGUID)
	SLATE_ARGUMENT(bool, IsLoot)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_EVENT( FSelectedRowPickerLootObjectSignature, OnClicked )
		
SLATE_END_ARGS();
	FText GetText() const;
	void Construct(const FArguments& InArgs);
	
	FName GetLootObjectGUID() const {return LootObjectGUID;};
	
	void SetSelected(bool InSelected);
protected:
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	FSlateColor GetColor() const;
	FName LootObjectGUID;
	bool Selected = false;
	bool IsLoot = true;
	TSharedPtr<SButton> CategoryBTN;
	FReply Clicked();
	FText LootSpawner_Name;
	FSelectedRowPickerLootObjectSignature OnClicked;
	
};
