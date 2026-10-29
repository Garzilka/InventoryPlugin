#include "W_ProgressiveCategory.h"

#include "EditorStyleSet.h"
#include "PropertyHandle.h"
#include "W_ProgressiveRandomGenerationData.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootDataWidget/RandomGeneration/W_RandomGenerationData.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/LootPreset/Probable/W_ProbableGenerationData.h"
#include "Widgets/Layout/SExpandableArea.h"

void SW_ProgressiveCategory::Construct(const FArguments& InArgs)
{
    PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
    LootAssetEditor = InArgs._LootAssetEditorRef;
    ElementIdx = InArgs._ElementIdx;
	
	
    PropertyHandle_CategoryName = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveDataCell, CategoryName));
    if (!PropertyHandle_CategoryName.IsValid()) return;
	
    PropertyHandle_GenerationType = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveDataCell, TypeGeneration));
    if (!PropertyHandle_GenerationType.IsValid()) return;
	
    PropertyHandle_RandomGeneration = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveDataCell, RandomGenerationSettings));
    if (!PropertyHandle_RandomGeneration.IsValid()) return;
	
    PropertyHandle_ProbableGeneration = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProgressiveDataCell, ProbableGenerationSettings));
    if (!PropertyHandle_ProbableGeneration.IsValid()) return;

    ChildSlot
    [
        SNew(SBox)
        .Padding(2.5f)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
            [
                SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                .BorderBackgroundColor(FLinearColor(0.4f, 0.4f, 0.4f))
                .Padding(5.f)
                [
                    SAssignNew(CategoryExpand, SExpandableArea)
                    .InitiallyCollapsed(true)
                    .HeaderContent()
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                            .Text( this, &SW_ProgressiveCategory::GetText )
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
                                .OnClicked(this, &SW_ProgressiveCategory::SIGNATURE_BTN_REMOVE)
                            ]
                        ]
                    ]
                    .BodyContent()
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .Padding(0.f, 10.f, 0.f, 0.f)
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                    PropertyHandle_CategoryName->CreatePropertyNameWidget()
                            ]
                            + SHorizontalBox::Slot()
                            .Padding(5.f, 0.f, 0.f, 0.f)
                            .AutoWidth()
                            [
                                    PropertyHandle_CategoryName->CreatePropertyValueWidget()
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
                                    PropertyHandle_GenerationType->CreatePropertyNameWidget()
                            ]
                            + SHorizontalBox::Slot()
                            .Padding(5.f, 0.f, 0.f, 0.f)
                            .AutoWidth()
                            [
                                    PropertyHandle_GenerationType->CreatePropertyValueWidget()
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(5.f, 5.f, 0.f, 0.f)
                        .AutoHeight()
                        [
                            SNew(SBox)
                            .Visibility(this, &SW_ProgressiveCategory::GetVisibilityRandom)
                            [
                                SNew(SW_ProgressiveRandomGenerationData)
                                .LootAssetEditorRef(LootAssetEditor)
                                .PropertyHandle_Struct(PropertyHandle_RandomGeneration)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(5.f, 5.f, 0.f, 0.f)
                        .AutoHeight()
                        [
                            SNew(SBox)
                            .Visibility(this, &SW_ProgressiveCategory::GetVisibilityProbable)
                            [
                                SNew(SW_ProbableGenerationData)
                                .LootAssetEditorRef(LootAssetEditor)
                                .PropertyHandle_Struct(PropertyHandle_ProbableGeneration)
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}

FText SW_ProgressiveCategory::GetText() const
{
	FString Result;
	PropertyHandle_CategoryName->GetValue(Result);
	return FText::FromString(Result);
}

EVisibility SW_ProgressiveCategory::GetVisibilityRandom() const
{
	uint8 Enum;
	PropertyHandle_GenerationType->GetValue(Enum);
	EProgressiveGenType Type = static_cast<EProgressiveGenType>(Enum);
	return (Type == EProgressiveGenType::EPGT_Random ? EVisibility::Visible : EVisibility::Collapsed);
}

EVisibility SW_ProgressiveCategory::GetVisibilityProbable() const
{
	uint8 Enum;
	PropertyHandle_GenerationType->GetValue(Enum);
	EProgressiveGenType Type = static_cast<EProgressiveGenType>(Enum);
	return (Type == EProgressiveGenType::EPGT_Probable ? EVisibility::Visible : EVisibility::Collapsed);
}

FReply SW_ProgressiveCategory::SIGNATURE_BTN_REMOVE()
{
    OnClicked.ExecuteIfBound(ElementIdx);	
    return FReply::Handled();
}