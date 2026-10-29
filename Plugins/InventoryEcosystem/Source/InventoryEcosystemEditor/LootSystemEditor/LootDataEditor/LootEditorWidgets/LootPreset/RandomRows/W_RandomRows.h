#pragma once
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"

class SW_RandomRows : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_RandomRows)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_RandomRows() {};
	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> CreateArray();
	FReply AddButtonClicked() ;

protected:
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	
	TSharedPtr<SExpandableArea> EA_Rows;
	TSharedPtr<SVerticalBox> VB_Array;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_Rows;
	TSharedPtr<IPropertyHandle> PropertyHandle_AmountSettings;
};
