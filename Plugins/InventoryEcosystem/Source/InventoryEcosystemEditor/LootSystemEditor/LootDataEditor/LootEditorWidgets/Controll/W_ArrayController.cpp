#include "W_ArrayController.h"

#include "EditorStyleSet.h"
#include "PropertyHandle.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_Add.h"
#include "InventoryEcosystemEditor/InventoryCoreEditor/Widgets/Buttons/ArrayControll/S_BTN_ClearAll.h"
#include "Widgets/Layout/SExpandableArea.h"

void SW_ArrayController::Construct(const FArguments& InArgs)
{
	PropertyHandle_Array = InArgs._ArrayRef;
	CustomWidget = InArgs._CustomWidget;
	LootAssetEditor = InArgs._LootAssetEditorRef;
	DisplayName = InArgs._DisplayName;
	PropertyHandle_OwnerStruct = InArgs._PropertyHandle_OwnerStruct;
	
	if (!PropertyHandle_Array.IsValid()) return;

	SAssignNew(DisplayNamePlace, SVerticalBox);
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(InArgs._EdgingColor)
		[
			SNew(SBorder)
			.Padding(1.f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(InArgs._BackgroundColor)
			[
				SAssignNew(CategoryExpand, SExpandableArea)
				.HeaderContent()
				[
					SNew(SBox)
					.MinDesiredHeight(15.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SBox)
							.VAlign(EVerticalAlignment::VAlign_Center)
							[
								DisplayNamePlace.ToSharedRef()
							]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_Add)
							.OnClicked(this, &SW_ArrayController::AddButtonClicked)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f, 0.f, 0.f)
						[
							SNew(SW_BTN_ClearAll)
							.OnClicked(this, &SW_ArrayController::RemoveAll)
						]
					]
				]
				.BodyContent()
				[
					SNew(SBox)
					.Padding(5.f)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							CreateArray()
						]
						+SVerticalBox::Slot()
						.Padding(0.f, 5.f, 0.f, 0.f)
						.AutoHeight()
						[
							CreateAdditional()
						]
					]
				]
			]
		]
	];
	if (CustomWidget)
	{
		DisplayNamePlace->AddSlot()
		[
			CustomWidget.ToSharedRef()
		];
		return;
	}
	DisplayNamePlace->AddSlot()
	[
		SNew(STextBlock)
		.Text(DisplayName)
	];
}

void SW_ArrayController::RemoveAt(int32 Id)
{
	TSharedPtr<IPropertyHandleArray> InPropertyHandle_Array = PropertyHandle_Array->AsArray();
	if (InPropertyHandle_Array.IsValid())
	{
		InPropertyHandle_Array->DeleteItem(Id);
	}
	else
	{
		auto InPropertyHandle_Map = PropertyHandle_Array->AsMap();
		if (InPropertyHandle_Map.IsValid())
		{
			InPropertyHandle_Map->DeleteItem(Id);
		}
	}
	CreateArray();
}

FReply SW_ArrayController::RemoveAll()
{
	TSharedPtr<IPropertyHandleArray> InPropertyHandle_Array = PropertyHandle_Array->AsArray();
	if (InPropertyHandle_Array.IsValid())
	{
		InPropertyHandle_Array->EmptyArray();
	}
	else
	{
		auto InPropertyHandle_Map = PropertyHandle_Array->AsMap();
		if (InPropertyHandle_Map.IsValid())
		{
			InPropertyHandle_Map->Empty();
		}
	}
	CreateArray();
	return FReply::Handled();
}

TSharedRef<SW_BaseArrayElement> SW_ArrayController::CreateArrayElement(TSharedPtr<IPropertyHandle> InPropertyHandle_Array, int32 Idx,
	TWeakPtr<FLootEditorAssetEditor> InLootAssetEditor)
{
	return SNew(SW_BaseArrayElement);
};

FReply SW_ArrayController::AddButtonClicked()
{	
	TSharedPtr<IPropertyHandleArray> InPropertyHandle_Array = PropertyHandle_Array->AsArray();
	if (InPropertyHandle_Array.IsValid())
	{
		InPropertyHandle_Array->AddItem();
	}
	else
	{
		auto InPropertyHandle_Map = PropertyHandle_Array->AsMap();
		if (InPropertyHandle_Map.IsValid())
		{
			InPropertyHandle_Map->AddItem();
		}
	}
	CreateArray();
	return FReply::Handled();
}

TSharedRef<SWidget> SW_ArrayController::CreateArray()
{	
	if (!VB_Array.IsValid())
	{
		VB_Array.Reset();
		VB_Array = SNew(SVerticalBox);
	}
	else
	{
		VB_Array->ClearChildren();
	}

	if (!PropertyHandle_Array.IsValid()) return VB_Array.ToSharedRef();
	
	uint32 Size = 0;
	TSharedPtr<IPropertyHandleArray> InPropertyHandle_Array = PropertyHandle_Array->AsArray();
	if (InPropertyHandle_Array.IsValid())
	{
		InPropertyHandle_Array->GetNumElements(Size);
	}
	else
	{
		auto InPropertyHandle_Map = PropertyHandle_Array->AsMap();
		if (InPropertyHandle_Map.IsValid())
		{
			InPropertyHandle_Map->GetNumElements(Size);
		}
	}

	for (uint32 Idx = 0; Idx < Size; Idx++)
	{
		TSharedRef<SW_BaseArrayElement> Element = CreateArrayElement(PropertyHandle_Array, Idx, LootAssetEditor);
		Element->OnClicked.BindRaw(this, &SW_ArrayController::RemoveAt);
		
		VB_Array->AddSlot()
		.AutoHeight()
		[
			Element
		];
	}
	
	return VB_Array.ToSharedRef();
}
