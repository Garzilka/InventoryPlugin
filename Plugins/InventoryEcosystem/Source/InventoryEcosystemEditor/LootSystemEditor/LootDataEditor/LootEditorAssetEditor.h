#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "ISinglePropertyView.h"
#include "Tools.h"
#include "Misc/NotifyHook.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

/**
 * 
 */
namespace LootEditorAppModes
{
	const FName LootSettings("Loot Settings");
	const FName LootPreset("Loot Preset");
	const FName LootData("Loot Data");
	const FName LootProfile("Loot Profile");
}

namespace LootEditorTabs
{
	const FName LootSettings_List("Loot Settings List");
	const FName LootSettings_Workflow("Loot Settings Workflow");
	
	const FName LootPreset_List("Loot Preset List");
	const FName LootPreset_Workflow("Loot Preset Workflow");
	
	const FName LootData_List("Loot Data List");
	const FName LootData_Workflow("Loot Data Workflow");
	const FName LootData_ItemList("Loot Data Item List");
	
	const FName LootProfile_List("Loot Profile List");
	const FName LootProfile_Workflow("Loot Profile Workflow");
}


DECLARE_DELEGATE(FChangedNameOrCategorySignature)
DECLARE_DELEGATE_OneParam(FNewSelectedLootObjectSignature, FName)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FObjectChangedCategorySignature, FName, FName, FName)



class ULootData;
class LOOTSYSTEMEDITOR_API FLootEditorAssetEditor : public FWorkflowCentricApplication, public FNotifyHook, public FGCObject
{
	
public:

	FNewSelectedLootObjectSignature OnNewSelectedSettingsDataObject;
	FNewSelectedLootObjectSignature OnNewSelectedLootDataObject;
	FNewSelectedLootObjectSignature OnNewSelectedLootPressetObject;
	FChangedNameOrCategorySignature OnNeedRebuilLootData;
	FChangedNameOrCategorySignature OnNeedRebuilLootPresset;
	FChangedNameOrCategorySignature OnNeedRebuilSettingData;
	FObjectChangedCategorySignature OnObjectChangedCategory;
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	void InitLootAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, ULootData* InLootData);
	void BuildToolBar(FToolBarBuilder& ToolbarBuilder);
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

	

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void RegisterToolBarTab(const TSharedRef<FTabManager>& InTabManager);

	TSharedRef<SWidget> SpawnTab_Graph();
	TSharedRef<SWidget> SpawnTab_CategoryList();

	TObjectPtr<ULootData> GetLootDataRef() { return LootDataRef;}

	void Event_NewCurrentSelectedBTN(class SLootObjectBTN* Object);
	void Event_ObjectChangeCategory(ECategorySelector ObjectType, FName ObjectGUID, FName OldCategory, FName NewCategory);
	void Event_DeleteObject(ECategorySelector ObjectType, FName ObjectGUID);
	
    void EventUpdateLootData();
	
	FCategoryObjectData CurrentCategoryLootData;
	FCategoryObjectData CurrentCategoryLootSettings;
	FCategoryObjectData CurrentCategoryLootPreset;
	TSharedPtr<ISinglePropertyView> GetLootDataAssetProperty();
	TSharedPtr<ISinglePropertyView> GetSettingsDataAssetProperty();
	TSharedPtr<ISinglePropertyView> GetPressetDataAssetProperty();


	void RebuildCategoryData(ECategorySelector ObjectType);
	void BuildCategory(FCategoryObjectData* Target, FName Key, FName Path);
private:	
	TSharedPtr<ISinglePropertyView> SingleProperty_LootData;
	TSharedPtr<ISinglePropertyView> SingleProperty_SettingData;
	TSharedPtr<ISinglePropertyView> SingleProperty_PressetData;
	TObjectPtr<ULootData> LootDataRef{ nullptr };
	TSharedPtr<class SLootObjectBTN> CurrentSelectedBTN;
};
