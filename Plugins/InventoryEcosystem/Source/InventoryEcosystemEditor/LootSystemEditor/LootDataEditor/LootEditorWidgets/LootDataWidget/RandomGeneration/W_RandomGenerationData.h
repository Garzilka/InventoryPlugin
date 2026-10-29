#pragma once
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_ArrayController.h"
#include "Widgets/Input/SEditableTextBox.h"

class SW_AC_RandomGeneration : public SW_ArrayController
{
public:
	virtual TSharedRef<SW_BaseArrayElement> CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx, TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor) override;
};

class SW_RandomGenerationData : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_RandomGenerationData)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_RandomGenerationData() {};
	void Construct(const FArguments& InArgs);
protected:
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_ArrayCells;
	TSharedPtr<IPropertyHandle> PropertyHandle_AmountSettings;
};
