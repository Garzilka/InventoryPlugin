#pragma once

#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "RowPickerCategoryList/W_RowPickerCategoryList.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SScrollBox.h"

DECLARE_DELEGATE_OneParam(FSelectObjectSignature, class SW_RowPickerObjectBTN*)
class SMyComboMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyComboMenu)
		: _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>()),
		_IsLoot(true)
	{}

	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
	SLATE_ARGUMENT(bool, IsLoot)
	SLATE_EVENT( FSelectObjectSignature, OnClicked )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		LootAssetEditor = InArgs._LootAssetEditorRef;
		OnClicked = InArgs._OnClicked;
		IsLoot = InArgs._IsLoot;
	
		ChildSlot
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(CB_RowPicker, SVerticalBox)
			]		
		];
		
		FCategoryObjectData& Data = (IsLoot ? LootAssetEditor.Pin()->CurrentCategoryLootData : LootAssetEditor.Pin()->CurrentCategoryLootPreset);
		
		for (auto CategoryData : Data.Children)
		{
			FCategoryObjectData NextCategory = (CategoryData.Value.Get() != nullptr ? *CategoryData.Value.Get() : FCategoryObjectData());
			CB_RowPicker->AddSlot()
			.AutoHeight()
			[
				SNew(SW_RowPickerCategoryList)
				.CategoryData(NextCategory)
				.CurrentPath(CategoryData.Key)
				.CurrentRoot(CategoryData.Key)
				.LootAssetEditorRef(LootAssetEditor)
				.IsLoot(IsLoot)
				.OnClicked(this, &SMyComboMenu::EventSelectedButtonCallback)
			];
		}
	}
	
	void EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID);
	
	FSelectObjectSignature OnClicked;
	TSharedPtr<SVerticalBox> CB_RowPicker;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	bool IsLoot = true;
};

class SW_LootPicker : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_LootPicker)
		:	_PropertyHandle_Struct(nullptr),
			_LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
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
	TSharedPtr<IPropertyHandle> PropertyHandle_Row;
	TSharedPtr<IPropertyHandle> PropertyHandle_Weight;
};
