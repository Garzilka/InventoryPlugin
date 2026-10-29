#pragma once
#include "PropertyHandle.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SExpandableArea.h"

class FLootEditorAssetEditor;

class SLootSettingMainWidget : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SLootSettingMainWidget)
		: _LootObjectGUID(FName("")),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(FName, LootObjectGUID)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SLootSettingMainWidget() {};
	void Construct(const FArguments& InArgs);

	void Init(FName NewObjectGUID);
	void SetDefault();
protected:
	void UpdateHandleMap();
	FReply AddButtonClicked() { return FReply::Handled();};
	bool IsTrueChanged = false;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SVerticalBox> VB_LootData;
	TSharedPtr<SBox> Container;
	TSharedPtr<SEditableTextBox> SETB_Category;
	TSharedPtr<SEditableTextBox> SETB_Name;
	TSharedPtr<STextBlock> STB_GUID;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_HandleMap;
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerateSettings;
	TSharedPtr<IPropertyHandle> PropertyHandle_IsPreset;
	TSharedPtr<IPropertyHandle> PropertyHandle_Pair;
	
	FName LootObjectGUID;
	FText LootDataObject_Name;
	FName LootDataObject_Category;
        void CategoryChanged(const FText& NewCategory, ETextCommit::Type InType);
	void NameChanged(const FText& NewName, ETextCommit::Type InType);
	EVisibility IsCustomSetting() const;
	EVisibility IsPreset() const;
	TSharedRef<SWidget> CreateArray();
};
