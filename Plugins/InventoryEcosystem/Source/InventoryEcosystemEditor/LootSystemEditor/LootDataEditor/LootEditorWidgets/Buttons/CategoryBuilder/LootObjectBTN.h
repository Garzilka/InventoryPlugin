#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FSelectedLootObjectSignature, class SLootObjectBTN*)

class SLootObjectBTN : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SLootObjectBTN)
		: _LootObjectGUID(FName("")),
		 _CategoryType(ECategorySelector::None),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
		SLATE_ARGUMENT(FName, LootObjectGUID)
		SLATE_ARGUMENT(ECategorySelector, CategoryType)
		SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		/** Called when the button is clicked */
		SLATE_EVENT( FSelectedLootObjectSignature, OnClicked )
		
SLATE_END_ARGS();
	FText GetText() const;
	void Construct(const FArguments& InArgs);
	
	FName GetLootObjectGUID() const {return LootObjectGUID;};
	
	void SetSelected(bool InSelected);
	ECategorySelector GetCategoryType() { return CategoryType; };
protected:
	FReply SIGNATURE_BTN_CLEAR();
    
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	FSlateColor GetColor() const;
	FName LootObjectGUID;
	ECategorySelector CategoryType;
	bool Selected = false;
	TSharedPtr<SButton> CategoryBTN;
	TSharedPtr<SW_BTN_ClearAll> RemoveBTN;
	FReply Clicked();
	FText LootSpawner_Name;
	/** The delegate to execute when the button is clicked */
	FSelectedLootObjectSignature OnClicked;
	
};
