#include "W_SettingBlock.h"

#include "EditorStyleSet.h"
#include "SGameplayTagCombo.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Controll/W_StandardizedRow.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/AmountGenerateSettings/W_AmountGenerateSettings.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/Probable/W_ProbableGenerationData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/Progressive/W_Progressive.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/RandomRows/W_RandomRows.h"
#include "Widgets/Layout/SExpandableArea.h"


void SW_SettingBlock::Construct(const FArguments& InArgs)
{
    PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
    LootAssetEditor = InArgs._LootAssetEditorRef;
    ElementIdx = InArgs._ElementIdx;
    OnClicked = InArgs._OnClicked;

    PropertyHandle_EnableSetting	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, EnableSetting));
    if (!PropertyHandle_EnableSetting.IsValid()) return;

    PropertyHandle_LootTag	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, LootTag));
    if (!PropertyHandle_LootTag.IsValid()) return;

    PropertyHandle_GenerateType			= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, GenerateType));
    if (!PropertyHandle_GenerateType.IsValid()) return;
	
    PropertyHandle_ProbableGeneration	= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, ProbableGenerationSettings));
    if (!PropertyHandle_ProbableGeneration.IsValid()) return;
	
    PropertyHandle_RandomGeneration		= PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, RandomGenerateSettings));
    if (!PropertyHandle_RandomGeneration.IsValid()) return;
	
    PropertyHandle_ProgressiveGeneration = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGenerateSettingsBlocks, ProgressiveGenerationSettings));
    if (!PropertyHandle_ProgressiveGeneration.IsValid()) return;

    ChildSlot
    [
        SNew(SBox)
        .Padding(5.f)
        [
            SNew(SBorder)
            .Padding(2.5f)
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                .BorderBackgroundColor(FLinearColor(0.4f, 0.4f, 0.4f))
                .Padding(5.f)
                [
                	SNew(SVerticalBox)
                	+ SVerticalBox::Slot()
                	.Padding(5.f, 5.f, 0.f, 0.f)
                	.AutoHeight()
                	[
                		SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .Padding(0.f)
                        .AutoWidth()
                        [
                            PropertyHandle_EnableSetting->CreatePropertyValueWidget()
                        ]
                        + SHorizontalBox::Slot()
                        .Padding(5.f, 0.f, 0.f, 0.f)
                        .AutoWidth()
                        [
                            SNew(SBox)
                            .Visibility(this, &SW_SettingBlock::OnGetLootTagVisibility)
                            [
                            	SNew(SGameplayTagCombo)
								.PropertyHandle(PropertyHandle_LootTag)
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .Padding(5.f, 0.f, 0.f, 0.f)
                        .AutoWidth()
                        [
                            SNew(SBox)
                            .HeightOverride(20.f)
                            .WidthOverride(20.f)
                            .VAlign(VAlign_Center)
                            .HAlign(HAlign_Center)
                            [
                            	SNew(SW_BTN_ClearAll)
                            	.OnClicked(this, &SW_SettingBlock::SIGNATURE_BTN_REMOVE)
                            ]
                        ]
                	]
                	+ SVerticalBox::Slot()
                	.AutoHeight()
                	.Padding(5.f, 5.f, 0.f, 0.f)
                	[
						SNew(SW_StandardizedRow)
						.DisplayName(PropertyHandle_GenerateType->GetPropertyDisplayName())
						.PropertyHandle(PropertyHandle_GenerateType)
                	]
                	+ SVerticalBox::Slot()
                	.AutoHeight()
                	.Padding(5.f, 5.f, 0.f, 0.f)
                	[
                		CreateArray()
                	]
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget> SW_SettingBlock::CreateArray()
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

	
	VB_LootData->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		.Visibility(this, &SW_SettingBlock::GetVisibilityProbable)
		[
			SNew(SW_ProbableGenerationData)
			.LootAssetEditorRef(LootAssetEditor)
			.PropertyHandle_Struct(PropertyHandle_ProbableGeneration)
		]
	];
	
	VB_LootData->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		.Visibility(this, &SW_SettingBlock::GetVisibilityRandom)
		[
			SNew(SW_RandomRows)
			.LootAssetEditorRef(LootAssetEditor)
			.PropertyHandle_Struct(PropertyHandle_RandomGeneration)
		]
	];
	
	VB_LootData->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		.Visibility(this, &SW_SettingBlock::GetVisibilityRandomProgressive)
		[
			SNew(SW_Progressive)
			.LootAssetEditorRef(LootAssetEditor)
			.PropertyHandle_Struct(PropertyHandle_ProgressiveGeneration)
		]
	];
	
	return VB_LootData.ToSharedRef();
}


FReply SW_SettingBlock::SIGNATURE_BTN_REMOVE()
{
    OnClicked.ExecuteIfBound(ElementIdx);	
    return FReply::Handled();
}

EVisibility SW_SettingBlock::OnGetLootTagVisibility() const
{
	uint8 Enum;
	PropertyHandle_EnableSetting->GetValue(Enum);
	EBlockEnableType Type = static_cast<EBlockEnableType>(Enum);
	return (Type == EBlockEnableType::EBET_Enabled ? EVisibility::Collapsed : EVisibility::Visible);
}

EVisibility SW_SettingBlock::GetVisibilityRandom() const
{
	uint8 Enum;
	PropertyHandle_GenerateType->GetValue(Enum);
	EGenerateType Type = static_cast<EGenerateType>(Enum);
	return (AsType(EGenerateType::EGIT_Random) ? EVisibility::Visible : EVisibility::Collapsed);
}

EVisibility SW_SettingBlock::GetVisibilityRandomProgressive() const
{
	return (AsType(EGenerateType::EGIT_Progressive) ? EVisibility::Visible : EVisibility::Collapsed);
}

EVisibility SW_SettingBlock::GetVisibilityProbable() const
{
	return (AsType(EGenerateType::EGIT_Probable) ? EVisibility::Visible : EVisibility::Collapsed);
}

bool SW_SettingBlock::AsType(EGenerateType Target) const
{
	uint8 Enum;
	PropertyHandle_GenerateType->GetValue(Enum);
	return ( Target == static_cast<EGenerateType>(Enum));
}