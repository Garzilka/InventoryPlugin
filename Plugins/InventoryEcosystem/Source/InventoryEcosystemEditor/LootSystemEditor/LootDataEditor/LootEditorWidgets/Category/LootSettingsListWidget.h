#pragma once
#include "CoreMinimal.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Tools.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/Factoryes/LootSettingsTabs/LootEditorTabFactory_LootSettingsList.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Buttons/CategoryBuilder/LootObjectBTN.h"
#include "Widgets/SCompoundWidget.h"




class SLootSettingsListWidget : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SLootSettingsListWidget)
		: _CategoryData(FCategoryObjectData()),
		  _CategoryType(ECategorySelector::None),
		  _CurrentPath(NAME_None),
		  _CurrentRoot(NAME_None),
		 _LootAssetEditorRef(TWeakPtr<FLootEditorAssetEditor>())
	{}
	SLATE_ARGUMENT(FCategoryObjectData, CategoryData)
	SLATE_ARGUMENT(ECategorySelector, CategoryType)
	SLATE_ARGUMENT(FName, CurrentPath)
	SLATE_ARGUMENT(FName, CurrentRoot)
	SLATE_ARGUMENT(TWeakPtr<FLootEditorAssetEditor>, LootAssetEditorRef)
		
SLATE_END_ARGS();
	void Construct(const FArguments& InArgs);

protected:
	void CheckChangeObject(FName ObjectGUID, FName OldCategory, FName NewCategory);
	FCategoryObjectData CategoryData;
	FName CurrentPath;
	FName CurrentRoot;
	ECategorySelector CategoryType;
	TSharedPtr<SVerticalBox> CurrentLootObjects;
	TWeakPtr<FLootEditorAssetEditor> LootAssetEditor;
	TSharedPtr<SExpandableArea> CategoryExpand;
	void EventSelectedButton(SLootObjectBTN* ObjectGUID);

	TSharedRef<SWidget> CreateChildren(FCategoryObjectData Category, int32 Idx);
	TSharedRef<SWidget> CreateArray(TArray<FName> LootSettings);
	
};
