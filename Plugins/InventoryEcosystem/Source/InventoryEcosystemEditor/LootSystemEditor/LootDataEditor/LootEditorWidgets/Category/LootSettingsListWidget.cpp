#include "LootSettingsListWidget.h"

#include "EditorStyleSet.h"

#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Buttons/CategoryBuilder/CategoryBTN.h"
#include "Widgets/Layout/SExpandableArea.h"
void SLootSettingsListWidget::Construct(const FArguments& InArgs)
{
    CategoryData = InArgs._CategoryData;
    LootAssetEditor = InArgs._LootAssetEditorRef;
    CategoryType = InArgs._CategoryType;
    CurrentPath = InArgs._CurrentPath;
    CurrentRoot = InArgs._CurrentRoot;

    if (CategoryType == ECategorySelector::Data)
    {
        LootAssetEditor.Pin()->OnObjectChangedCategory.AddRaw(this, &SLootSettingsListWidget::CheckChangeObject);	
    }

    this->ChildSlot
	.Padding(5.f, 0.f, 0.f, 0.f)
    [
    	SAssignNew(CategoryExpand, SExpandableArea)
    	.InitiallyCollapsed(true)
    	.HeaderContent()
    	[
    	    SNew(SBox)
    	    .VAlign(VAlign_Top)
    	    .HAlign(HAlign_Left)
    	    [
    	        SNew(SCategoryBTN)
    	        .CategoryName(CurrentRoot)
    	    ]
    	]
    	.BodyContent()
    	[
    	    SNew(SBox)
    	    .Padding(10.f, 0.f, 0.f, 0.f)
    	    [
    	        SNew(SHorizontalBox)
    	        + SHorizontalBox::Slot()
    	        .AutoWidth()
    	        [
    	            SNew(SBox)
    	            .WidthOverride(3.f)
    	            .MinDesiredHeight(3.f)
    	            .Padding(0.f, 0.f, 0.f, 5.f)
    	        	[
    	        		SNew(SVerticalBox)
    					+ SVerticalBox::Slot()
    					[
    						SNew(SBorder)
    						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
    						.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f))
    					]
    	        		+ SVerticalBox::Slot()
    					.Padding(0.f, 0.f, 0.f, 0.f)
    					.AutoHeight()
    					[
    						SNew(SBox)
    						.HeightOverride(10.f)
    					]
    	            ]
    	        ]
    	        + SHorizontalBox::Slot()
    	        [
    	            SNew(SVerticalBox)
    	            + SVerticalBox::Slot()
    	            .AutoHeight()
    	            [
    	                CreateChildren(CategoryData, 0)
    	            ]
    	            + SVerticalBox::Slot()
    	            .Padding(0.f, 5.f, 0.f, 2.f)
    	            .AutoHeight()
    	            [
    	                SNew(SBox)
    	                .VAlign(VAlign_Top)
    	                .HAlign(HAlign_Left)
    	                [
    	                	CreateArray(CategoryData.ObjectGUIDS)
    	                ]
    	            ]
    	        ]
    	    ]				
    	]
    ];
}

void SLootSettingsListWidget::CheckChangeObject(FName ObjectGUID, FName OldCategory, FName NewCategory)
{
    if (!CurrentLootObjects.IsValid()) return;
	
    if (CurrentPath == OldCategory)
    {
        auto Data = Tools::FindByPath(LootAssetEditor.Pin()->CurrentCategoryLootData.Children, Tools::GetPathFromString(CurrentPath.ToString())).Get();
        if (!Data) return;
        CategoryData = *Data;
        CreateArray(CategoryData.ObjectGUIDS);
    }
	
    if (CurrentPath == NewCategory)
    {
        auto Data = Tools::FindByPath(LootAssetEditor.Pin()->CurrentCategoryLootData.Children, Tools::GetPathFromString(CurrentPath.ToString())).Get();
        if (!Data) return;
        CategoryData = *Data;
        CreateArray(CategoryData.ObjectGUIDS);
    }
}

TSharedRef<SWidget> SLootSettingsListWidget::CreateArray(TArray<FName> InLootSettings)
{
	if (!CurrentLootObjects.IsValid())
	{
		CurrentLootObjects.Reset();
		CurrentLootObjects = SNew(SVerticalBox);
	}
	else
	{
		CurrentLootObjects->ClearChildren();
	}
	for (FName ObjectGUID : InLootSettings)		
	{
		CurrentLootObjects->AddSlot()
		.AutoHeight()
		.Padding(0.f, 0.f, 0.f, 5.f)
		[
			SNew(SBox)
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SNew(SLootObjectBTN)
				.LootObjectGUID(ObjectGUID)
				.CategoryType(CategoryType)
				.LootAssetEditorRef(LootAssetEditor)
				.OnClicked(this, &SLootSettingsListWidget::EventSelectedButton)
			]
		];
	}
	return CurrentLootObjects.ToSharedRef();
}


void SLootSettingsListWidget::EventSelectedButton(SLootObjectBTN* ObjectGUID)
{
	LootAssetEditor.Pin()->Event_NewCurrentSelectedBTN(ObjectGUID);
}

TSharedRef<SWidget> SLootSettingsListWidget::CreateChildren(FCategoryObjectData Category, int32 Idx)
{
	int32 Next = Idx + 1;
	TSharedRef<SVerticalBox> ResultBox = SNew(SVerticalBox);
	FName NextPath = NAME_None;
	for (auto Data : Category.Children)
	{
		FCategoryObjectData NextCategory = (Data.Value.Get() != nullptr ? *Data.Value.Get() : FCategoryObjectData());
		if(CurrentPath == NAME_None)
		{
			NextPath = Data.Key;
		}
		else
		{
			NextPath = FName(CurrentPath.ToString() + "|" + Data.Key.ToString());
		}

		ResultBox->AddSlot()
		.Padding(0.f, 3.f, 0.f, 0.f)
		.AutoHeight()
		[
			SNew(SBox)
			[
				SNew(SBorder)
				.Padding(1.f)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.3f,0.3f,0.3f))
				[
					SNew(SLootSettingsListWidget)
					.CategoryData(NextCategory)
					.CategoryType(CategoryType)
					.CurrentPath(NextPath)
					.CurrentRoot(Data.Key)
					.LootAssetEditorRef(LootAssetEditor)
				]
				
			]
		];
	}
	
	return ResultBox;
}