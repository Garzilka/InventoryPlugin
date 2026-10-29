#include "LootObjectBTN.h"

#include "EditorStyleSet.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "Application/SlateApplicationBase.h"
#include "Widgets/Accessibility/SlateAccessibleMessageHandler.h"

FText SLootObjectBTN::GetText() const
{
	return LootSpawner_Name;
}

void SLootObjectBTN::Construct(const FArguments& InArgs)
{
    LootObjectGUID = InArgs._LootObjectGUID;
    LootAssetEditor = InArgs._LootAssetEditorRef;
    CategoryType = InArgs._CategoryType;
    OnClicked = InArgs._OnClicked;

    if (CategoryType == ECategorySelector::Data)
    {
        if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootData().Find(LootObjectGUID))
        {
            LootSpawner_Name = TextData->MetaData.Object_Name;
        }
    }
    if (CategoryType == ECategorySelector::Settings)
    {
        if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootSettingsData().Find(LootObjectGUID))
        {
            LootSpawner_Name = TextData->MetaData.Object_Name;
        }
    }
    if (CategoryType == ECategorySelector::Preset)
    {
        if (auto TextData = LootAssetEditor.Pin()->GetLootDataRef()->GetLootPresets().Find(LootObjectGUID))
        {
            LootSpawner_Name = TextData->MetaData.Object_Name;
        }
    }
	
    this->ChildSlot
    [
        SNew(SBox)
        .HeightOverride(21.f)
        .MinDesiredWidth(130.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .HeightOverride(0.5f)
                .WidthOverride(15.f)
                .VAlign(VAlign_Center)
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                    .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew(CategoryBTN, SButton)
                .ButtonStyle( FCoreStyle::Get(), "NoBorder")
                .ClickMethod( EButtonClickMethod::MouseDown )
                .OnClicked( this, &SLootObjectBTN::Clicked )
                .ContentPadding(0.f)
                .ForegroundColor( FSlateColor::UseForeground() )
                .IsFocusable( false )
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                    .BorderBackgroundColor(this, &SLootObjectBTN::GetColor)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SBox)
                            .Padding(2.f, 0.f, 2.f, 0.f)
                            .VAlign(VAlign_Center)
                            .HAlign(HAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text( this, &SLootObjectBTN::GetText )
                            ]
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(3.f, 0.f, 0.f, 0.f)
                        [
                            SNew(SBox)
                            .Padding(2.f, 0.f, 2.f, 0.f)
                            .HeightOverride(15.f)
                            .WidthOverride(19.f)
                            .VAlign(VAlign_Center)
                            .HAlign(HAlign_Center)
                            [
                                SNew(SW_BTN_ClearAll)
                                .OnClicked(this, &SLootObjectBTN::SIGNATURE_BTN_CLEAR)
                            ]
                        ]  
                    ]
                ]
            ]
            
        ]
    ];
}

void SLootObjectBTN::SetSelected(bool InSelected)
{
	Selected = InSelected;
}

FReply SLootObjectBTN::SIGNATURE_BTN_CLEAR()
{
    LootAssetEditor.Pin()->Event_DeleteObject(CategoryType, LootObjectGUID);
    return FReply::Handled();
}

FSlateColor SLootObjectBTN::GetColor() const
{
    if (Selected)
    {
        return ((CategoryBTN.Get() && CategoryBTN.Get()->IsHovered()) ? FColor::FromHex("ffa929") : FColor::FromHex("d48d22"));
    }
    return ((CategoryBTN.Get() && CategoryBTN.Get()->IsHovered()) ? FLinearColor(0.0f, 0.8f, 1.f) : FLinearColor(0.1f, 0.6f, 0.75f));
}

FReply SLootObjectBTN::Clicked()
{
    if (OnClicked.IsBound())
    {
        OnClicked.Execute(this);
		
        FReply Reply = FReply::Handled();
#if WITH_ACCESSIBILITY
        FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(FSlateAccessibleMessageHandler::FSlateWidgetAccessibleEventArgs(AsShared(), EAccessibleEvent::Activate));
#endif
        return Reply;
    }
    else
    {
        return FReply::Handled();
    }
}
