#pragma once

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_ArrayController.h"
#include "Widgets/SCompoundWidget.h"

class SW_AC_Progressive: public SW_ArrayController
{
public:
	virtual TSharedRef<SW_BaseArrayElement> CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor) override;
};

class SW_Progressive : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_Progressive)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);
protected:    
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;

	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_ProgressiveData;
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerateRules;
};
