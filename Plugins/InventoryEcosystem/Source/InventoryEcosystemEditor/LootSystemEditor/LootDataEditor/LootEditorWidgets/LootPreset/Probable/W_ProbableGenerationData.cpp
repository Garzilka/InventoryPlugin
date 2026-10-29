#include "W_ProbableGenerationData.h"

#include "W_ProbableGenerationCell.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "Widgets/Layout/SExpandableArea.h"


void SW_ProbableGenerationData::Construct(const FArguments& InArgs)
{
	PropertyHandle_Struct = InArgs._PropertyHandle_Struct;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	
	
	PropertyHandle_ArrayCells = PropertyHandle_Struct->GetChildHandle(GET_MEMBER_NAME_CHECKED(FProbableGenerationSettings, GenerateCells));
	if (!PropertyHandle_ArrayCells.IsValid()) return;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(0.f, 0.f, 0.f, 5.f)
		.AutoHeight()
		[
			SAssignNew(CategoryExpand, SExpandableArea)
			.HeaderContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Array"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 0.f, 0.f, 0.f)
				[
					SNew(SW_BTN_Add)
					.OnClicked(this, &SW_ProbableGenerationData::AddButtonClicked)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 0.f, 0.f, 0.f)
				[
					SNew(SW_BTN_ClearAll)
					.OnClicked(this, &SW_ProbableGenerationData::Event_RemoveAllCell)
				]
			]
			.BodyContent()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.f, 10.f, 0.f, 0.f)
				.AutoHeight()
				[
					CreateArray()
				]
			]
		]
	];
}

FReply SW_ProbableGenerationData::AddButtonClicked()
{
	if (!PropertyHandle_ArrayCells.IsValid())
	{
		return FReply::Handled();
	}

	PropertyHandle_ArrayCells->AsArray()->AddItem();
	CreateArray();
	return FReply::Handled();
}

TSharedRef<SWidget> SW_ProbableGenerationData::CreateArray()
{
	if (!CurrentArrayWidget.IsValid())
	{
		CurrentArrayWidget.Reset();
		CurrentArrayWidget = SNew(SVerticalBox);
	}
	else
	{
		CurrentArrayWidget->ClearChildren();
	}
	uint32 childrenCount = 0;
	PropertyHandle_ArrayCells->AsArray()->GetNumElements(childrenCount);

	for (uint32 Idx = 0; Idx < childrenCount; ++Idx)
	{
		TSharedRef<IPropertyHandle> LPropertyHandle_Cell = PropertyHandle_ArrayCells->AsArray()->GetElement(Idx);
		
		CurrentArrayWidget->AddSlot()
		.AutoHeight()
		.Padding(5.f, 0.f, 0.f, 8.f)
		[
			SNew(SBox)
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			[
			    SNew(SW_ProbableGenerationCell)
			    .LootAssetEditorRef(LootAssetEditor)
			    .PropertyHandle_Struct(LPropertyHandle_Cell)
                            .Cell_ID(Idx)
                            .OnClicked(this, &SW_ProbableGenerationData::Event_RemoveCell)
			]
		];
	}
	return CurrentArrayWidget.ToSharedRef();
}

void SW_ProbableGenerationData::Event_RemoveCell(int32 Id)
{
    if (!PropertyHandle_ArrayCells.IsValid()) return;
    if (!PropertyHandle_ArrayCells->AsArray().IsValid()) return;
	
    PropertyHandle_ArrayCells->AsArray()->DeleteItem(Id);
    CreateArray();
}

FReply SW_ProbableGenerationData::Event_RemoveAllCell()
{
    if (!PropertyHandle_ArrayCells.IsValid()) return FReply::Handled();
    if (!PropertyHandle_ArrayCells->AsArray().IsValid()) return FReply::Handled();
	
    PropertyHandle_ArrayCells->AsArray()->EmptyArray();
    CreateArray();
    return FReply::Handled();
}