#pragma once

#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorAssetEditor.h"

class SW_RespawnData : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_RespawnData)
		: _PropertyHandle_Struct(nullptr),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Struct)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();

	LOOTSYSTEMEDITOR_API SW_RespawnData() {};
	
	LOOTSYSTEMEDITOR_API FText GetText() const;
        EVisibility GetVisibilityTimeRespawn() const;
	void Construct(const FArguments& InArgs);
protected:
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<IPropertyHandle> PropertyHandle_Struct;
    
	TSharedPtr<IPropertyHandle> PropertyHandle_TimeToRestart;
	TSharedPtr<IPropertyHandle> PropertyHandle_RespawnLootType;
};
