#pragma once
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/Input/SEditableTextBox.h"

class SW_AmountGenerateSettings : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_AmountGenerateSettings)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_AmountGenerateSettings() {};
	void Construct(const FArguments& InArgs);
protected:
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	TSharedPtr<SVerticalBox> CurrentArrayWidget;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TSharedPtr<IPropertyHandle> PropertyHandle_Min;
	TSharedPtr<IPropertyHandle> PropertyHandle_Max;
	TSharedPtr<IPropertyHandle> PropertyHandle_TypeOfGenerate;
	TSharedPtr<IPropertyHandle> PropertyHandle_TypeTypeCheckClass;
};
