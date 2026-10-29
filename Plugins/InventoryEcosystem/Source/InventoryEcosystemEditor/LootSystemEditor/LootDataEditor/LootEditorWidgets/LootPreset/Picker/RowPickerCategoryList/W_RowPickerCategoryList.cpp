#include "W_RowPickerCategoryList.h"

#include "EditorStyleSet.h"
#include "W_RowPickerObjectBTN.h"
#include "Widgets/Layout/SExpandableArea.h"

void SW_RowPickerCategoryList::Construct(const FArguments& InArgs)
{
	CategoryData = InArgs._CategoryData;
	// LootAssetEditor = InArgs._LootAssetEditorRef;
	CurrentPath = InArgs._CurrentPath;
	CurrentRoot = InArgs._CurrentRoot;
	OnClicked = InArgs._OnClicked;
	IsLoot = InArgs._IsLoot;

	this->ChildSlot
	[
		SNew(SBox)
		.Padding(10.f, 0.f, 0.f, 0.f)
		[
			SNew(SBox)
			.WidthOverride(300.f)
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SAssignNew(CategoryExpand, SExpandableArea)
				.InitiallyCollapsed(true)
				.HeaderContent()
				[
					SNew(SBox)
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text( this, &SW_RowPickerCategoryList::GetText )
					]
				]
				.BodyContent()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						CreateChildren(CategoryData, 0)
					]
					+ SVerticalBox::Slot()
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

FText SW_RowPickerCategoryList::GetText() const
{
	return FText::FromName(CurrentRoot);
}

void SW_RowPickerCategoryList::CheckChangeObject(FName ObjectGUID, FName OldCategory, FName NewCategory)
{
	if (!CurrentLootObjects.IsValid()) return;
	
	// if (CurrentPath == OldCategory)
	// {
	// 	FCategoryObjectData LCategoryData = (IsLoot ? LootAssetEditor.Pin()->CurrentCategoryLootData : LootAssetEditor.Pin()->CurrentCategoryLootPreset);
	// 	auto Data = Tools::FindByPath(LCategoryData.Children, Tools::GetPathFromString(CurrentPath.ToString())).Get();
	// 	if (!Data) return;
	// 	CategoryData = *Data;
	// 	CreateArray(CategoryData.ObjectGUIDS);
	// }
	
	// if (CurrentPath == NewCategory)
	// {
	// 	FCategoryObjectData LCategoryData = (IsLoot ? LootAssetEditor.Pin()->CurrentCategoryLootData : LootAssetEditor.Pin()->CurrentCategoryLootPreset);
	// 	auto Data = Tools::FindByPath(LCategoryData.Children, Tools::GetPathFromString(CurrentPath.ToString())).Get();
	// 	if (!Data) return;
	// 	CategoryData = *Data;
	// 	CreateArray(CategoryData.ObjectGUIDS);
	// }
}

TSharedRef<SWidget> SW_RowPickerCategoryList::CreateArray(TArray<FName> InLootSettings)
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
	// for (FName ObjectGUID : InLootSettings)		
	// {
	// 	CurrentLootObjects->AddSlot()
	// 	.AutoHeight()
	// 	.Padding(5.f, 3.f, 0.f, 3.f)
	// 	[
	// 		SNew(SBox)
	// 		.VAlign(VAlign_Top)
	// 		.HAlign(HAlign_Left)
	// 		[
	// 			SNew(SW_RowPickerObjectBTN)
	// 			.LootObjectGUID(ObjectGUID)
	// 			.LootAssetEditorRef(LootAssetEditor)
	// 			.IsLoot(IsLoot)
	// 			.OnClicked(this, &SW_RowPickerCategoryList::EventSelectedButton)
	// 		]
	// 	];
	// }
	return CurrentLootObjects.ToSharedRef();
}

void SW_RowPickerCategoryList::EventSelectedButton(SW_RowPickerObjectBTN* ObjectGUID)
{
	OnClicked.ExecuteIfBound(ObjectGUID);
}

void SW_RowPickerCategoryList::EventSelectedButtonCallback(SW_RowPickerObjectBTN* ObjectGUID)
{
	OnClicked.ExecuteIfBound(ObjectGUID);
}

TSharedRef<SWidget> SW_RowPickerCategoryList::CreateChildren(FCategoryObjectData Category, int32 Idx)
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
		.AutoHeight()
		[
			SNew(SBox)
			[
				SNew(SW_RowPickerCategoryList)
				.CategoryData(NextCategory)
				.CurrentPath(NextPath)
				.CurrentRoot(Data.Key)
				.LootAssetEditorRef(LootAssetEditor)
				.IsLoot(IsLoot)
				.OnClicked(this, &SW_RowPickerCategoryList::EventSelectedButtonCallback)
			]
		];
	}
	
	return ResultBox;
}
