#include "LootSettingMainWidget.h"

#include "ISinglePropertyView.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/LootDataMainWidget.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/W_LootPreset.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/Picker/W_PresetPicker.h"
#include "Modules/ModuleManager.h"



void SLootSettingMainWidget::Construct(const FArguments& InArgs)
{
    LootObjectGUID = InArgs._LootObjectGUID;
    LootAssetEditor = InArgs._LootAssetEditorRef;
	
    
    if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootSettingsData().Find(LootObjectGUID))
    {
        LootDataObject_Name = TextData->MetaData.Object_Name;
        LootDataObject_Category = TextData->MetaData.Category;
    }
    IsTrueChanged = false;
    SETB_Category = SNew(SEditableTextBox)
            .Text(LootDataObject_Name)
            .OnTextCommitted(this, &SLootSettingMainWidget::CategoryChanged);
	
    SETB_Name = SNew(SEditableTextBox)
            .OnTextCommitted(this, &SLootSettingMainWidget::NameChanged)
            .Text(FText::FromString(LootDataObject_Category.ToString()));
	
    IsTrueChanged = true;
    Container = SNew(SBox)
        .Padding(10.f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew(STB_GUID, STextBlock)
                .Text(FText::FromString(LootObjectGUID.ToString()))
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SBox)
                    .WidthOverride(150.f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("Category"))
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SETB_Category.ToSharedRef()
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(0.f, 10.f, 0.f, 0.f)
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SBox)
                    .WidthOverride(150.f)
                    [
                            SNew(STextBlock)
                            .Text(FText::FromString("Name"))
                    ]
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SETB_Name.ToSharedRef()
                ]
            ]
            + SVerticalBox::Slot()
            .Padding(0.f, 10.f, 0.f, 5.f)
            .AutoHeight()
            [
                CreateArray()
            ]
        ];
    this->ChildSlot
    .Padding(FMargin(0))
    [
            Container.ToSharedRef()
    ];
    if (LootObjectGUID.IsValid() && !LootObjectGUID.IsNone())
    {
        Init(LootObjectGUID);
    }
    else
    {
        SetDefault();
    }
}

void SLootSettingMainWidget::Init(FName NewObjectGUID)
{
	if (NewObjectGUID == LootObjectGUID) return;
	LootObjectGUID = NewObjectGUID;
	
	IsTrueChanged = false;
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootSettingsData().Find(LootObjectGUID))
	{
		LootDataObject_Name = TextData->MetaData.Object_Name;
		LootDataObject_Category = TextData->MetaData.Category;
	}
	
	SETB_Category.ToSharedRef()->SetText(FText::FromString(LootDataObject_Category.ToString()));
	SETB_Name.ToSharedRef()->SetText(LootDataObject_Name);
	STB_GUID.ToSharedRef()->SetText(FText::FromString(LootObjectGUID.ToString()));

	
	UpdateHandleMap();	
	uint32 childrenCount = 0;
	PropertyHandle_HandleMap->AsMap()->GetNumElements(childrenCount);

	for (uint32 Idx = 0; Idx < childrenCount; ++Idx)
	{
		TSharedRef<IPropertyHandle> LPropertyHandle_Pair = PropertyHandle_HandleMap->AsMap()->GetElement(Idx);
	
		FName KeyName;
		LPropertyHandle_Pair->GetKeyHandle()->GetValue(KeyName);		
		if (KeyName != LootObjectGUID) continue;
		
		PropertyHandle_Pair = LPropertyHandle_Pair;
		PropertyHandle_GenerateSettings	=	PropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, GenerateSettings));
		PropertyHandle_IsPreset =			PropertyHandle_Pair->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLootSettingsData, bUsePreset));
		break;
	}
	CreateArray();
	IsTrueChanged = true;
}

void SLootSettingMainWidget::SetDefault()
{
	PropertyHandle_GenerateSettings = nullptr;
	PropertyHandle_IsPreset = nullptr;
	PropertyHandle_Pair = nullptr;
	IsTrueChanged = false;
	
	SETB_Category.ToSharedRef()->SetText(FText::FromString(""));
	SETB_Name.ToSharedRef()->SetText(FText::FromString(""));
	STB_GUID.ToSharedRef()->SetText(FText::FromString(""));
	IsTrueChanged = true;
	
	if (VB_LootData.IsValid())
	{
		VB_LootData->ClearChildren();
	}
}

void SLootSettingMainWidget::UpdateHandleMap()
{
	if (LootAssetEditor.Pin() && LootAssetEditor.Pin()->GetSettingsDataAssetProperty())
	{
		PropertyHandle_HandleMap = LootAssetEditor.Pin()->GetSettingsDataAssetProperty()->GetPropertyHandle();
	}
}

void SLootSettingMainWidget::CategoryChanged(const FText& NewCategory, ETextCommit::Type InType)
{
	if (!IsTrueChanged) return;
	
	FName LOldCategory = LootDataObject_Category;
	FName LNewCategory = FName(NewCategory.ToString());
	if (LOldCategory == LNewCategory) return;
	
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootSettingsData().Find(LootObjectGUID))
	{
		TextData->MetaData.Category = LNewCategory;
	}
	if (!LootObjectGUID.IsValid()) return;
	
	LootDataObject_Category = LNewCategory;
	LootAssetEditor.Pin()->Event_ObjectChangeCategory(ECategorySelector::Settings, LootObjectGUID, LOldCategory, LNewCategory);
}

void SLootSettingMainWidget::NameChanged(const FText& NewName, ETextCommit::Type InType)
{
	if (!IsTrueChanged) return;
	
	if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootSettingsData().Find(LootObjectGUID))
	{
		TextData->MetaData.Object_Name = NewName;
	}

	if (!LootObjectGUID.IsValid()) return;
	
	LootAssetEditor.Pin()->Event_ObjectChangeCategory(ECategorySelector::Settings, LootObjectGUID, LootDataObject_Category, LootDataObject_Category);
}

EVisibility SLootSettingMainWidget::IsCustomSetting() const
{
	if (!PropertyHandle_IsPreset.IsValid()) return EVisibility::Collapsed;

	bool IsPreset;
	PropertyHandle_IsPreset->GetValue(IsPreset);
	return (IsPreset ? EVisibility::Collapsed : EVisibility::Visible);
}

EVisibility SLootSettingMainWidget::IsPreset() const
{
	if (!PropertyHandle_IsPreset.IsValid()) return EVisibility::Collapsed;
	
	bool IsPreset;
	PropertyHandle_IsPreset->GetValue(IsPreset);
	return (IsPreset ? EVisibility::Visible : EVisibility::Collapsed);
}

TSharedRef<SWidget> SLootSettingMainWidget::CreateArray()
{
	if (!VB_LootData.IsValid())
	{
		VB_LootData.Reset();
		VB_LootData = SNew(SVerticalBox);
	}
	else
	{
		VB_LootData->ClearChildren();
	}
	
	if (PropertyHandle_GenerateSettings.IsValid())
	{
		VB_LootData->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				PropertyHandle_IsPreset->CreatePropertyNameWidget()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f, 0.f, 0.f)
			[
				PropertyHandle_IsPreset->CreatePropertyValueWidget()
			]			
		];
		
		VB_LootData->AddSlot()
		.AutoHeight()
		[
			SNew(SBox)
			.Visibility(this, &SLootSettingMainWidget::IsCustomSetting)
			[
				SNew(SW_LootPreset)
				.LootAssetEditorRef(LootAssetEditor)
				.PropertyHandle_Struct(PropertyHandle_GenerateSettings)
			]
			
		];
		VB_LootData->AddSlot()
		.AutoHeight()
		[
			SNew(SBox)
			.Visibility(this, &SLootSettingMainWidget::IsPreset)
			[
				SNew(SW_PresetPicker)
				.LootAssetEditorRef(LootAssetEditor)
				.PropertyHandle_Struct(PropertyHandle_Pair)
			]
		];
	}

	return VB_LootData.ToSharedRef();
}
