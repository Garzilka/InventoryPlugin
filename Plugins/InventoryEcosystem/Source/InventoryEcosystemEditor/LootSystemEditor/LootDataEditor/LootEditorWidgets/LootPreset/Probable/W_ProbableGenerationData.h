#pragma once

#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/SCompoundWidget.h"

class SW_ProbableGenerationData : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ProbableGenerationData)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_ProbableGenerationData() {};
	void Construct(const FArguments& InArgs);
	FReply AddButtonClicked();
	TSharedRef<SWidget> CreateArray();
protected:
    void Event_RemoveCell(int32 Id);
    FReply Event_RemoveAllCell();
    
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	TSharedPtr<SVerticalBox> CurrentArrayWidget;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_ArrayCells;
	TSharedPtr<IPropertyHandle> PropertyHandle_AmountSettings;
};
