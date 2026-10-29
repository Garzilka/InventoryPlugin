#include "W_ProbableGenerationCell.h"

#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Item/ItemContainer/W_ItemContainer.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Item/Selector/W_ItemSelector.h"

void SW_ProbableGenerationCell::Construct(const FArguments& InArgs)
{
    PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
    LootAssetEditor = InArgs._LootAssetEditorRef;
    OnClicked = InArgs._OnClicked;
    Cell_ID = InArgs._Cell_ID;
	
	
    PropertyHandle_Percent = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProbableGenerationCell, Percent));
    if (!PropertyHandle_Percent.IsValid()) return;
	
    PropertyHandle_ItemContext = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProbableGenerationCell, GenerateCell));
    if (!PropertyHandle_ItemContext.IsValid()) return;
	
    PropertyHandle_bUseCustomLogic = PropertyHandle_ItemContext->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMakeItemContext, bIsCustom));
    if (!PropertyHandle_bUseCustomLogic.IsValid()) return;
	
    PropertyHandle_GenerateClass = PropertyHandle_ItemContext->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMakeItemContext, MakeItemRule));
    if (!PropertyHandle_GenerateClass.IsValid()) return;
	
    PropertyHandle_ItemData = PropertyHandle_ItemContext->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMakeItemContext, ItemData));
    if (!PropertyHandle_ItemData.IsValid()) return;
	
    PropertyHandle_ItemClass = PropertyHandle_ItemData->GetChildHandle(GET_MEMBER_NAME_CHECKED(FInventoryItemData, ItemClass));
    if (!PropertyHandle_ItemClass.IsValid()) return;
	
    PropertyHandle_bIsRange = PropertyHandle_ItemData->GetChildHandle(GET_MEMBER_NAME_CHECKED(FInventoryItemData, bIsRange));
    if (!PropertyHandle_bIsRange.IsValid()) return;
	
    PropertyHandle_Amount = PropertyHandle_ItemData->GetChildHandle(GET_MEMBER_NAME_CHECKED(FInventoryItemData, Amount));
    if (!PropertyHandle_Amount.IsValid()) return;
	
    PropertyHandle_Range_X = PropertyHandle_ItemData->GetChildHandle(GET_MEMBER_NAME_CHECKED(FIntPoint, X));
    if (!PropertyHandle_Range_X.IsValid()) return;
	
    PropertyHandle_Range_Y = PropertyHandle_ItemData->GetChildHandle(GET_MEMBER_NAME_CHECKED(FIntPoint, Y));
    if (!PropertyHandle_Range_Y.IsValid()) return;

    ChildSlot
    [
        SNew(SBox)
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FColor::FromHex("191919FF"))
            [
                SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                .BorderBackgroundColor(FColor::FromHex("5e5e5e"))
                .Padding(3.f)
                [
                    SNew(SBox)
                    .Padding(5.f)
                    [
                        SNew(SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SHorizontalBox)
                            + SHorizontalBox::Slot()
                            .AutoWidth()
                            [
                                PropertyHandle_bUseCustomLogic->CreatePropertyValueWidget()
                            ]
                            + SHorizontalBox::Slot()
                            .Padding(5.f, 0.f, 0.f, 0.f)
                            .AutoWidth()
                            [
                            	SNew(SBox)
                            	.Visibility(this, &SW_ProbableGenerationCell::GetVisibilityGenerateClass)
                            	[
                            		PropertyHandle_GenerateClass->CreatePropertyValueWidget()
                            	]
                            ]
                            + SHorizontalBox::Slot()
                            .Padding(5.f, 0.f, 0.f, 0.f)
                            .AutoWidth()
                            [
                            	SNew(SBox)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Visibility(this, &SW_ProbableGenerationCell::GetVisibilityItemClass)
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									[
										SNew(SW_ItemViewContainer)
										.PropertyHandle_Item(PropertyHandle_ItemClass)
									]
									+ SHorizontalBox::Slot()
									.AutoWidth()
									.Padding(0.f)
									[
										SNew(SW_ItemSelector)
										.LootAssetEditorRef(LootAssetEditor)
										.PropertyHandle_Item(PropertyHandle_ItemClass)
									]   
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
                                	.OnClicked(this, &SW_ProbableGenerationCell::SIGNATURE_BTN_REMOVE)
                                ]
                            ]
                        ]
                        +SVerticalBox::Slot()
                        .Padding(0.f, 8.f, 0.f, 0.f)
                        .AutoHeight()
                        [
                                SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                        SNew(SVerticalBox)
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                                SNew(SHorizontalBox)
                                                + SHorizontalBox::Slot()
                                                .Padding(0.f, 0.f, 5.f, 0.f)
                                                .AutoWidth()
                                                [
                                                        SNew(STextBlock)
                                                        .Text(FText::FromString("Percent: "))
                                                ]
                                                + SHorizontalBox::Slot()
                                                .AutoWidth()
                                                [
                                                        PropertyHandle_Percent->CreatePropertyValueWidget()
                                                ]
                                        ]
                                ]
                        ]
                        + SVerticalBox::Slot()
                        .Padding(0.f, 8.f, 0.f, 0.f)
                        .AutoHeight()
                        [
                                SNew(SBox)
                                .Visibility(this, &SW_ProbableGenerationCell::GetVisibilityItemClass)
                                [
                                        SNew(SHorizontalBox)
                                        + SHorizontalBox::Slot()
                                        .Padding(0.f, 0.f, 5.f, 0.f)
                                        .AutoWidth()
                                        [
                                                SNew(STextBlock)
                                                .Text(FText::FromString("Amount: "))
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        [
                                                SNew(SBox)
                                                .Visibility(this, &SW_ProbableGenerationCell::GetVisibilityAmount)
                                                [
                                                        PropertyHandle_Amount->CreatePropertyValueWidget()
                                                ]
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        [
                                                SNew(SBox)
                                                .Visibility(this, &SW_ProbableGenerationCell::GetVisibilityRange)
                                                [
                                                        PropertyHandle_Range_X->CreatePropertyValueWidget()
                                                ]
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        [
                                                SNew(SBox)
                                                .Visibility(this, &SW_ProbableGenerationCell::GetVisibilityRange)
                                                [
                                                        PropertyHandle_Range_Y->CreatePropertyValueWidget()
                                                ]
                                        ]
                                        + SHorizontalBox::Slot()
                                        .Padding(15.f, 0.f, 10.f, 0.f)
                                        .AutoWidth()
                                        [
                                                PropertyHandle_bIsRange->CreatePropertyValueWidget()
                                        ]
                                ]
                        ]
                    ]
                ]
            ]
        ]
    ];
}

EVisibility SW_ProbableGenerationCell::GetVisibilityGenerateClass() const
{
	if (!PropertyHandle_bUseCustomLogic.IsValid()) return EVisibility::Collapsed;
	bool Value = false;
	PropertyHandle_bUseCustomLogic->GetValue(Value);
	return (Value ? EVisibility::Visible : EVisibility::Collapsed);
}

EVisibility SW_ProbableGenerationCell::GetVisibilityItemClass() const
{
	if (!PropertyHandle_bUseCustomLogic.IsValid()) return EVisibility::Collapsed;
	bool Value = false;
	PropertyHandle_bUseCustomLogic->GetValue(Value);
	return (Value ? EVisibility::Collapsed : EVisibility::Visible);
}

EVisibility SW_ProbableGenerationCell::GetVisibilityAmount() const
{
	if (!PropertyHandle_bIsRange.IsValid()) return EVisibility::Collapsed;
	bool Value = false;
	PropertyHandle_bIsRange->GetValue(Value);
	return (Value ? EVisibility::Collapsed : EVisibility::Visible);
}

EVisibility SW_ProbableGenerationCell::GetVisibilityRange() const
{
	if (!PropertyHandle_bIsRange.IsValid()) return EVisibility::Collapsed;
	bool Value = false;
	PropertyHandle_bIsRange->GetValue(Value);
	return (Value ? EVisibility::Visible : EVisibility::Collapsed);
}

FReply SW_ProbableGenerationCell::SIGNATURE_BTN_REMOVE()
{
    OnClicked.ExecuteIfBound(Cell_ID);	
    return FReply::Handled();
}