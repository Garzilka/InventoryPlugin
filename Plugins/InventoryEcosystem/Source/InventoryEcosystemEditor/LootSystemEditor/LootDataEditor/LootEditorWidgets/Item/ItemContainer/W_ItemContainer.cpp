#include "W_ItemContainer.h"

#include "PropertyHandle.h"
#include "Engine/AssetManager.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/DragAndDropOperations/Item/Item_EditorDragAndDrop.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"


void SW_ItemViewContainer::Construct(const FArguments& InArgs)
{
	PropertyHandle_Item = InArgs._PropertyHandle_Item;

	FSimpleDelegate OnCategoryChange = FSimpleDelegate::CreateSP(this, &SW_ItemViewContainer::ValueChange);
	PropertyHandle_Item->SetOnPropertyValueChanged(OnCategoryChange);
	
	ValueChange();
	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(50)
		.HeightOverride(50)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Level.ColorIcon"))
			.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Level.ColorIcon"))
				.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
				[
					SNew(SBox)
					.Visibility(this, &SW_ItemViewContainer::GetVisibilityImage)
					.Padding(-1.f)
					[
						SNew(SScaleBox)
						.Stretch(EStretch::ScaleToFit)
						[
							SNew(SImage)
							.Image(this, &SW_ItemViewContainer::GetItemIcon)
						]
					]
				]
			]
		]
	];
}

FReply SW_ItemViewContainer::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragDropOperation> DragOp = DragDropEvent.GetOperation();
	if (!DragOp.IsValid()) return FReply::Unhandled();
	
	TSharedPtr<FItem_EditorDragAndDrop> MyOp = StaticCastSharedPtr<FItem_EditorDragAndDrop>(DragOp);
	if (!MyOp.IsValid()) return FReply::Unhandled();
			
	UAssetManager& AssetManager = UAssetManager::Get();
	
	PropertyHandle_Item->NotifyPreChange();
	TWeakObjectPtr<UBaseItem> InAssetData = nullptr;
	if (MyOp->ItemAssetData.IsValid())
	{
		FString ClassPathStr = MyOp->ItemAssetData->TagsAndValues.FindTag(TEXT("GeneratedClass")).GetValue();
		FString ObjectPath = FPackageName::ExportTextPathToObjectPath(ClassPathStr);
		
		UClass* Debug_ItemClass = StaticLoadClass(UBaseItem::StaticClass(), nullptr, *ObjectPath);
		if (Debug_ItemClass)
		{
			InAssetData = Debug_ItemClass->GetDefaultObject<UBaseItem>();
		}
	}

	UObject* ValueObject = nullptr;
	if (PropertyHandle_Item->GetValue(ValueObject) == FPropertyAccess::Success)
	{
		PropertyHandle_Item->NotifyPreChange();
		if (InAssetData.Get())
		{
			PropertyHandle_Item->SetValue(InAssetData->GetClass());
		}
		else
		{
			PropertyHandle_Item->ResetToDefault();
		}

		PropertyHandle_Item->NotifyPostChange(EPropertyChangeType::ValueSet);
	}
		
	
	return FReply::Unhandled();
};

void SW_ItemViewContainer::ValueChange()
{
	UObject* ValueObject = nullptr;
	if (PropertyHandle_Item->GetValue(ValueObject) == FPropertyAccess::Success)
	{
		UClass* ItemClass = Cast<UClass>(ValueObject);
		if (ItemClass)
		{
			TileItemRef = ItemClass->GetDefaultObject<UBaseItem>();
			if (TileItemRef && TileItemRef->GetIcon())
			{
				ItemBrush = FSlateBrush();
				ItemBrush.DrawAs = ESlateBrushDrawType::Type::Image;
				ItemBrush.ImageSize = FVector2D(16.f, 16.f);
				ItemBrush.SetResourceObject(TileItemRef->GetIcon());
			}
		}
		else
		{
			TileItemRef = nullptr;
		}
	}
}

const FSlateBrush* SW_ItemViewContainer::GetItemIcon() const
{
	if (TileItemRef && TileItemRef->GetIcon())
	{
		return &ItemBrush;
	}
	
	return FAppStyle::GetBrush("Level.ColorIcon");
}

EVisibility SW_ItemViewContainer::GetVisibilityImage() const
{
	return ((TileItemRef) ? EVisibility::Visible : EVisibility::Collapsed);
}