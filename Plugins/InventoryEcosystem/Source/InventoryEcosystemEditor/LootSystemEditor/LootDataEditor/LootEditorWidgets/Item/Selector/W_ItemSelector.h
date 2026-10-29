#pragma once
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboButton.h"

DECLARE_DELEGATE_OneParam(FSelectChangeSignature, class SW_ItemViewEntity*)

class SW_ItemSelectorMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SW_ItemSelectorMenu)
		: _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}

	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_EVENT( FSelectChangeSignature, OnClicked )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	void EventSelectedButtonCallback(SW_ItemViewEntity* ObjectGUID);
	
	FSelectChangeSignature OnClicked;
	TArray<TSharedPtr<FAssetData>> ListItems;
	TSharedPtr<SVerticalBox> CB_RowPicker;
	TSharedPtr<SVerticalBox> CB_MainMenu;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	void OnSearchBoxTextChanged(const FText& Text) ;
	FReply Clicked();

	void RebuildList();
};


class SW_ItemSelector : public SCompoundWidget
{
public:	
	
	SLATE_BEGIN_ARGS(SW_ItemSelector)
		:	_PropertyHandle_Item(nullptr),
		_LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
			
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Item)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> OnGetKeyContent() const;
	FText GetCurrentKeyDesc() const;
	void EventSelectedButtonCallback(SW_ItemViewEntity* ObjectGUID);
protected:	
	void ValueChange();
	UBaseItem* TileItemRef = nullptr;
	TSharedPtr<SVerticalBox> CB_RowPicker;
	TSharedPtr<SW_ItemSelectorMenu> CM_Menu;
	TSharedPtr<SComboButton> CB_Selector;
	TSharedPtr<IPropertyHandle> PropertyHandle_Item;
	
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	
};
