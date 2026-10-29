#include "LootEditorAssetEditor.h"

#include "EditorStyleSet.h"
#include "Tools.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "ApplicationMode/LootData/LootEditorAppMode_LootData.h"
#include "ApplicationMode/LootPreset/LootEditorAppMode_LootPreset.h"
#include "ApplicationMode/LootProfile/LootEditorAppMode_LootProfile.h"
#include "ApplicationMode/LootSettings/LootEditorAppMode_LootSettings.h"
#include "InventoryEcosystem/LootSystem/LootSystemSetting.h"
#include "LootEditorWidgets/Buttons/CategoryBuilder/LootObjectBTN.h"
#include "Modules/ModuleManager.h"


#pragma region ToolKit

FName FLootEditorAssetEditor::GetToolkitFName() const
{
	return FName("Loot Editor");
}

FText FLootEditorAssetEditor::GetBaseToolkitName() const
{
	return GetToolkitName();
}

FString FLootEditorAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("Loot Editor");
}

FLinearColor FLootEditorAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(FColor::FromHex("00AFFF"));
}

#pragma endregion

#pragma region Editor

void FLootEditorAssetEditor::EventUpdateLootData()
{
    RebuildCategoryData(ECategorySelector::Settings);
    OnNeedRebuilSettingData.ExecuteIfBound();
}

void FLootEditorAssetEditor::InitLootAssetEditor(const EToolkitMode::Type Mode,	const TSharedPtr<class IToolkitHost>& InitToolkitHost, ULootData* InLootData)
{
    LootDataRef = InLootData;
    LootDataRef->OnUpdateLootSettingData.BindRaw(this, &FLootEditorAssetEditor::EventUpdateLootData);
    
    RebuildCategoryData(ECategorySelector::Data);
    RebuildCategoryData(ECategorySelector::Settings);
    RebuildCategoryData(ECategorySelector::Preset);
    InitAssetEditor(Mode, InitToolkitHost, FName("Loot Editor"), FTabManager::FLayout::NullLayout, true, true, LootDataRef.Get(), false);
	
    TSharedRef<FExtender> ToolkitExtender = MakeShareable(new FExtender);
    ToolkitExtender->AddToolBarExtension("Asset", EExtensionHook::Position::After, GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP(this, &FLootEditorAssetEditor::BuildToolBar));
    AddToolbarExtender(ToolkitExtender);

	

    const  TSharedPtr<FTabManager::FLayout> LootSettings_ModeLayout = FTabManager::NewLayout("LootEditor_LootSeetings_Ver_2")
    ->AddArea
    (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
            FTabManager::NewStack()
                    ->SetSizeCoefficient(0.1f)
                    ->AddTab(NAME_None, ETabState::OpenedTab)
                    ->SetHideTabWell(true)
            )
            ->Split
            (
            FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Horizontal)
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.3f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootSettings_List, ETabState::OpenedTab)
                    )
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootSettings_Workflow, ETabState::OpenedTab)
                    )
            )
    );

    const  TSharedPtr<FTabManager::FLayout> LootPreset_ModeLayout = FTabManager::NewLayout("LootEditor_LootPreset_Ver_2")
    ->AddArea
    (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
            FTabManager::NewStack()
                    ->SetSizeCoefficient(0.1f)
                    ->AddTab(NAME_None, ETabState::OpenedTab)
                    ->SetHideTabWell(true)
            )
            ->Split
            (
            FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Horizontal)
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.3f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootPreset_List, ETabState::OpenedTab)
                    )
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootPreset_Workflow, ETabState::OpenedTab)
                    )
            )
    );

    const  TSharedPtr<FTabManager::FLayout> LootData_ModeLayout = FTabManager::NewLayout("LootEditor_LootData_Ver_4")
    ->AddArea
    (
    	FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split
		(
		FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.15f)
				->SetHideTabWell(true)
				->AddTab(LootEditorTabs::LootData_List, ETabState::OpenedTab)
				)
				->Split
				(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->SetHideTabWell(true)
				->AddTab(LootEditorTabs::LootData_Workflow, ETabState::OpenedTab)
				)
				->Split
				(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.15f)
				->SetHideTabWell(true)
				->AddTab(LootEditorTabs::LootData_ItemList, ETabState::OpenedTab)
				)
		)
    );

    const  TSharedPtr<FTabManager::FLayout> LootProfile_ModeLayout = FTabManager::NewLayout("LootEditor_LootProfile_Ver_2")
    ->AddArea
    (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
            FTabManager::NewStack()
                    ->SetSizeCoefficient(0.1f)
                    ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
                    ->SetHideTabWell(true)
            )
            ->Split
            (
            FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Horizontal)
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.3f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootProfile_List, ETabState::OpenedTab)
                    )
                    ->Split
                    (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->SetHideTabWell(true)
                    ->AddTab(LootEditorTabs::LootProfile_Workflow, ETabState::OpenedTab)
                    )
            )
    );
	
    AddApplicationMode(LootEditorAppModes::LootSettings, MakeShareable(new FLootEditorAppMode_LootSettings(SharedThis(this), LootEditorAppModes::LootSettings, LootSettings_ModeLayout)));
    AddApplicationMode(LootEditorAppModes::LootPreset, MakeShareable(new FLootEditorAppMode_LootPreset(SharedThis(this), LootEditorAppModes::LootPreset, LootPreset_ModeLayout)));
    AddApplicationMode(LootEditorAppModes::LootData, MakeShareable(new FLootEditorAppMode_LootData(SharedThis(this), LootEditorAppModes::LootData, LootData_ModeLayout)));
    AddApplicationMode(LootEditorAppModes::LootProfile, MakeShareable(new FLootEditorAppMode_LootProfile(SharedThis(this), LootEditorAppModes::LootProfile, LootProfile_ModeLayout)));

	

    SetCurrentMode(LootEditorAppModes::LootSettings);
}

void FLootEditorAssetEditor::BuildToolBar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.BeginSection("LootEditorModes");
	{
		// Edit Grid button
		ToolbarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FLootEditorAssetEditor::SetCurrentMode, LootEditorAppModes::LootSettings),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() {
					return GetCurrentMode() == LootEditorAppModes::LootSettings;
				})
			),
			NAME_None,
			FText::FromString("Loot Settings"),
			FText::FromString(""),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"),
			EUserInterfaceActionType::ToggleButton
		);

		ToolbarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FLootEditorAssetEditor::SetCurrentMode, LootEditorAppModes::LootPreset),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() {
					return GetCurrentMode() == LootEditorAppModes::LootPreset;
				})
			),
			NAME_None,
			FText::FromString("Loot Preset"),
			FText::FromString(""),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"),
			EUserInterfaceActionType::ToggleButton
		);

		ToolbarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FLootEditorAssetEditor::SetCurrentMode, LootEditorAppModes::LootData),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() {
					return GetCurrentMode() == LootEditorAppModes::LootData;
				})
			),
			NAME_None,
			FText::FromString("Loot Data"),
			FText::FromString(""),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"),
			EUserInterfaceActionType::ToggleButton
		);

		ToolbarBuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FLootEditorAssetEditor::SetCurrentMode, LootEditorAppModes::LootProfile),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() {
					return GetCurrentMode() == LootEditorAppModes::LootProfile;
				})
			),
			NAME_None,
			FText::FromString("Profile"),
			FText::FromString(""),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"),
			EUserInterfaceActionType::ToggleButton
		);
	}
	ToolbarBuilder.EndSection();
}

void FLootEditorAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	
	Collector.AddReferencedObject(LootDataRef);
}

FString FLootEditorAssetEditor::GetReferencerName() const
{
	if (IsValid(LootDataRef.Get()))
	{
		return LootDataRef.Get()->GetName();
	}
	return "Error Reference Name";
}
#pragma endregion

#pragma region TabSpawner

void FLootEditorAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FLootEditorAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);
}

void FLootEditorAssetEditor::RegisterToolBarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

TSharedRef<SWidget> FLootEditorAssetEditor::SpawnTab_Graph()
{
	return SNew(SBox);
}

TSharedRef<SWidget> FLootEditorAssetEditor::SpawnTab_CategoryList()
{
	return SNew(SBox);
}

void FLootEditorAssetEditor::Event_NewCurrentSelectedBTN(SLootObjectBTN* Object)
{
    if (CurrentSelectedBTN.IsValid())
    {
        CurrentSelectedBTN->SetSelected(false);
    }
	
    CurrentSelectedBTN = StaticCastSharedRef<SLootObjectBTN>(Object->AsShared());
    if (!CurrentSelectedBTN.IsValid()) return;
	
    CurrentSelectedBTN.Get()->SetSelected(true);
	
    if (CurrentSelectedBTN->GetCategoryType() == ECategorySelector::Settings)
    {
        OnNewSelectedSettingsDataObject.ExecuteIfBound(CurrentSelectedBTN->GetLootObjectGUID());
    }
    if (CurrentSelectedBTN->GetCategoryType() == ECategorySelector::Data)
    {
        OnNewSelectedLootDataObject.ExecuteIfBound(CurrentSelectedBTN->GetLootObjectGUID());
    }
    if (CurrentSelectedBTN->GetCategoryType() == ECategorySelector::Preset)
    {
        OnNewSelectedLootPressetObject.ExecuteIfBound(CurrentSelectedBTN->GetLootObjectGUID());
    }
}


void FLootEditorAssetEditor::Event_ObjectChangeCategory(ECategorySelector ObjectType, FName ObjectGUID, FName OldCategory, FName NewCategory)
{
    if (ObjectType == ECategorySelector::None) return;
	
    FCategoryObjectData& Target = (ObjectType == ECategorySelector::Data ? CurrentCategoryLootData : CurrentCategoryLootSettings);
    Target = (ObjectType == ECategorySelector::Settings ? CurrentCategoryLootSettings : CurrentCategoryLootPreset);
	
    TSharedPtr<FCategoryObjectData> OldPath = Tools::FindByPath(Target.Children, Tools::GetPathFromString(OldCategory.ToString()));
    TSharedPtr<FCategoryObjectData> NewPath = Tools::FindByPath(Target.Children, Tools::GetPathFromString(NewCategory.ToString()));
	
    //Смотрю достижимы ли пути в старой версии
    bool IsValid = OldPath.IsValid() && NewPath.IsValid();
    RebuildCategoryData(ObjectType);
	
    OldPath = Tools::FindByPath(Target.Children, Tools::GetPathFromString(OldCategory.ToString()));
    NewPath = Tools::FindByPath(Target.Children, Tools::GetPathFromString(NewCategory.ToString()));
	
    //Проверяю достижимы ли пути в новой и старой версиях
    IsValid &= NewPath.IsValid();
    if (IsValid) //Если да, тогда просто перенос объекта
    {
        OnObjectChangedCategory.Broadcast(ObjectGUID, OldCategory, NewCategory);
        return;
    }
    //Иначе пересборка категории
    OnObjectChangedCategory.Clear();
    if (ObjectType == ECategorySelector::Data)
    {
        OnNeedRebuilLootData.ExecuteIfBound();
    }
    if (ObjectType == ECategorySelector::Settings)
    {
        OnNeedRebuilSettingData.ExecuteIfBound();
    }
    if (ObjectType == ECategorySelector::Preset)
    {
        OnNeedRebuilLootPresset.ExecuteIfBound();
    }
}

void FLootEditorAssetEditor::Event_DeleteObject(ECategorySelector ObjectType, FName ObjectGUID)
{
    if (ULootSystemSetting::Get())
    {
        auto LootData = ULootSystemSetting::Get()->LootData;
        
        if (ObjectType == ECategorySelector::Data)
        {
            LootData->LootData.Remove(ObjectGUID);
            LootData->Modify();
            RebuildCategoryData(ObjectType);
            OnNeedRebuilLootData.ExecuteIfBound();
        }
        if (ObjectType == ECategorySelector::Settings)
        {
            LootData->LootSettings.Remove(ObjectGUID);
            LootData->Modify();
            RebuildCategoryData(ObjectType);
            OnNeedRebuilSettingData.ExecuteIfBound();
        }
        if (ObjectType == ECategorySelector::Preset)
        {
            LootData->LootPresets.Remove(ObjectGUID);
            LootData->Modify();
            RebuildCategoryData(ObjectType);
            OnNeedRebuilLootPresset.ExecuteIfBound();
        }
    }
}

void FLootEditorAssetEditor::RebuildCategoryData(ECategorySelector ObjectType)
{
    FCategoryObjectData* Target = nullptr;
	
    if (ObjectType == ECategorySelector::Data)
    {
        Target = &CurrentCategoryLootData;
    }
    if (ObjectType == ECategorySelector::Settings)
    {
        Target = &CurrentCategoryLootSettings;
    }
    if (ObjectType == ECategorySelector::Preset)
    {
        Target = &CurrentCategoryLootPreset;
    }
    if (!Target) return;
	
    *Target = FCategoryObjectData();

    if (ObjectType == ECategorySelector::Settings)
    {
        for (auto LData : LootDataRef->GetLootSettingsData())
        {
            BuildCategory(Target, LData.Key, LData.Value.MetaData.Category);
        }
    }

    if (ObjectType == ECategorySelector::Data)
    {
        for (auto LData : LootDataRef->GetLootData())
        {
            BuildCategory(Target, LData.Key, LData.Value.MetaData.Category);
        }
    }

    if (ObjectType == ECategorySelector::Preset)
    {
        for (auto LData : LootDataRef->GetLootPresets())
        {
            BuildCategory(Target, LData.Key, LData.Value.MetaData.Category);
        }
    }
}
void FLootEditorAssetEditor::BuildCategory(FCategoryObjectData* Target, FName InKey, FName InPath)
{
	FString NextSplit = InPath.ToString();
	FString Key = "|";
	FString Right = "";
	FString Left = "";
	
	if (!UKismetStringLibrary::Split(NextSplit, Key, Left, Right))
	{
		TSharedPtr<FCategoryObjectData> Category = Tools::AddChild(Target->Children, {FName(NextSplit)});
		if (Category.IsValid())
		{
			Category.Get()->ObjectGUIDS.Add(InKey);
		}
		return;
	}
		
	TSharedPtr<FCategoryObjectData> Category = Tools::AddChild(Target->Children, Tools::GetPathFromString(InPath.ToString()));
	if (Category.IsValid())
	{
		Category.Get()->ObjectGUIDS.Add(InKey);
	}
}



TSharedPtr<ISinglePropertyView> FLootEditorAssetEditor::GetLootDataAssetProperty()
{
	if (!SingleProperty_LootData.IsValid())
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		SingleProperty_LootData = PropertyModule.CreateSingleProperty(GetLootDataRef(),GET_MEMBER_NAME_CHECKED(ULootData, LootData),	FSinglePropertyParams());
	}
	return SingleProperty_LootData;
}

TSharedPtr<ISinglePropertyView> FLootEditorAssetEditor::GetSettingsDataAssetProperty()
{
	if (!SingleProperty_SettingData.IsValid())
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		SingleProperty_SettingData = PropertyModule.CreateSingleProperty(GetLootDataRef(),GET_MEMBER_NAME_CHECKED(ULootData, LootSettings),	FSinglePropertyParams());
	}
	return SingleProperty_SettingData;
}

TSharedPtr<ISinglePropertyView> FLootEditorAssetEditor::GetPressetDataAssetProperty()
{
	if (!SingleProperty_PressetData.IsValid())
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		SingleProperty_PressetData = PropertyModule.CreateSingleProperty(GetLootDataRef(),GET_MEMBER_NAME_CHECKED(ULootData, LootPresets),	FSinglePropertyParams());
	}
	return SingleProperty_PressetData;
}

#pragma endregion
