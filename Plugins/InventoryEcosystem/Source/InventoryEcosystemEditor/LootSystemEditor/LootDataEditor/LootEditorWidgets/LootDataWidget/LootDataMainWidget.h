#pragma once
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "Widgets/Input/SEditableTextBox.h"



class SLootDataMainWidget : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SLootDataMainWidget)
		: _LootObjectGUID(FName("")),
		 _LootAssetEditorRef(nullptr)
	{}
	SLATE_ARGUMENT(FName, LootObjectGUID)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);

	void Init(FName NewObjectGUID);
	void SetDefault();
protected:
	void UpdateHandleMap();
	void GenerateLootData();
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SVerticalBox> VB_LootData;
	TSharedPtr<SVerticalBox> B_MetaPlace;
	TSharedPtr<SExpandableArea> CategoryExpand;
	FName LootObjectGUID;

	
	TSharedPtr<IPropertyHandle> PropertyHandle_HandleMap;
	TSharedPtr<IPropertyHandle> PropertyHandle_RandomGeneration;
	TSharedPtr<IPropertyHandle> PropertyHandle_MetaData;
};
