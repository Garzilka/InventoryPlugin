#include "W_ItemViewEntity.h"

#include "AssetRegistry/AssetData.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/HorizontalBox.h"
#include "Engine/Texture2D.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/DragAndDropOperations/Item/Item_EditorDragAndDrop.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/STextBlock.h"


void SW_ItemViewEntity::Construct(const FArguments& InArgs, const TSharedPtr<FAssetData>& InAssetData)
{
	AssetData = InAssetData;
	OnClicked = InArgs._OnClicked;
	if (!AssetData.IsValid()) return;


	if (AssetData.IsValid())
	{
		FString ClassPathStr = AssetData->TagsAndValues.FindTag(TEXT("GeneratedClass")).GetValue();
		FString ObjectPath = FPackageName::ExportTextPathToObjectPath(ClassPathStr);
		
		UClass* Debug_ItemClass = StaticLoadClass(UBaseItem::StaticClass(), nullptr, *ObjectPath);
		if (Debug_ItemClass)
		{
			ItemRef = Debug_ItemClass->GetDefaultObject<UBaseItem>();
		}
	}
	
	ItemBrush = FSlateBrush();	
	if (ItemRef.IsValid() && ItemRef->GetIcon())
	{
		ItemBrush.DrawAs = ESlateBrushDrawType::Type::Image;
		ItemBrush.ImageSize = FVector2D(16.f, 16.f);
		ItemBrush.SetResourceObject(ItemRef->GetIcon());
	}
	
	ChildSlot
	[
		SAssignNew(CategoryBTN, SButton)
		.ButtonStyle( FCoreStyle::Get(), "NoBorder")
		.ClickMethod( EButtonClickMethod::MouseDown )
		.OnClicked( this, &SW_ItemViewEntity::Clicked )
		.ContentPadding(0.f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.IsFocusable( false )
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Level.ColorIcon"))
			.BorderBackgroundColor(this, &SW_ItemViewEntity::GetColor)
			.Padding(10)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				[
					SNew(SBox)
					.WidthOverride(40)
					.HeightOverride(40)
					[
						SNew(SScaleBox)
						.Stretch(EStretch::ScaleToFit)
						[
							SNew(SImage)
							.Image(this, &SW_ItemViewEntity::GetItemIcon)
						]
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				.Padding(10,0)
				[
					SNew(STextBlock).Text(FText::FromName(AssetData->AssetName))   
				]
			]
		]
	];
}

const FSlateBrush* SW_ItemViewEntity::GetItemIcon() const
{
	if (!ItemRef.IsValid())
	{
		return FAppStyle::GetBrush("Level.ColorIcon");
	}
	return &ItemBrush;
}

FReply SW_ItemViewEntity::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (AssetData.IsValid()) return FReply::Unhandled();
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		TSharedRef<FItem_EditorDragAndDrop> DragOp = FItem_EditorDragAndDrop::New(AssetData);
		return FReply::Handled().BeginDragDrop(DragOp);
	}

	return FReply::Unhandled();
}

FReply SW_ItemViewEntity::Clicked()
{
	if (OnClicked.IsBound())
	{
		OnClicked.ExecuteIfBound(this);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FSlateColor SW_ItemViewEntity::GetColor() const
{
	return ((CategoryBTN.Get() && CategoryBTN.Get()->IsHovered()) ? FLinearColor(FColor::FromHex("3F3F3F")) : FLinearColor(FColor::FromHex("2F2F2F")));
}