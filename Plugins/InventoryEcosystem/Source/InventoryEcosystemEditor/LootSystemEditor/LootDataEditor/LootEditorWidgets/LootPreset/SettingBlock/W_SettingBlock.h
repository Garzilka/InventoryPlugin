#pragma once

#include "PropertyHandle.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_BaseArrayElement.h"

DECLARE_DELEGATE_OneParam(FRemoveSettingBlockSignature, int32)

class SW_SettingBlock : public SW_BaseArrayElement
{
public:
	
	SLATE_BEGIN_ARGS(SW_SettingBlock)
		: _PropertyHandle_Struct(nullptr),
		 _ElementIdx(INDEX_NONE),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(int32, ElementIdx)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
        SLATE_EVENT( FRemoveSettingBlockSignature, OnClicked )
		
SLATE_END_ARGS();
	EVisibility OnGetLootTagVisibility() const;
	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> CreateArray();
	EVisibility GetVisibilityRandom() const;
	EVisibility GetVisibilityProbable() const;
	EVisibility GetVisibilityRandomProgressive() const;
	bool AsType(EGenerateType Target) const;

protected:
	FReply SIGNATURE_BTN_REMOVE();
    
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;

	TSharedPtr<SVerticalBox> VB_LootData;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
	FRemoveSettingBlockSignature OnClicked;
	
	TSharedPtr<IPropertyHandle> PropertyHandle_GenerateType;
	TSharedPtr<IPropertyHandle> PropertyHandle_RandomGeneration;
	TSharedPtr<IPropertyHandle> PropertyHandle_ProbableGeneration;
	TSharedPtr<IPropertyHandle> PropertyHandle_ProgressiveGeneration;

	TSharedPtr<IPropertyHandle> PropertyHandle_EnableSetting;
	TSharedPtr<IPropertyHandle> PropertyHandle_LootTag;
};
