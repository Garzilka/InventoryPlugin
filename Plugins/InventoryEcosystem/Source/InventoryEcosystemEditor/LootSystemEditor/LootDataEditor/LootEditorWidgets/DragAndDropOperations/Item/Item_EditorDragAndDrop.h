#pragma once
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DragAndDrop/DecoratedDragDropOp.h"
#include "Engine/Texture2D.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Item/Widget/W_ItemViewEntity.h"

class FItem_EditorDragAndDrop : public FDecoratedDragDropOp
{
public:
	DRAG_DROP_OPERATOR_TYPE(FMyDragDropOperation, FDecoratedDragDropOp)

	TSharedPtr<FAssetData> ItemAssetData = nullptr;
	
	static TSharedRef<FItem_EditorDragAndDrop> New(TSharedPtr<FAssetData> InAssetData)
	{
		TSharedRef<FItem_EditorDragAndDrop> Op = MakeShareable(new FItem_EditorDragAndDrop());
		Op->ItemAssetData = InAssetData;
		Op->Construct();
		return Op;
	}


	
	

	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
	{
		if (ItemAssetData.IsValid())
		{			
			return SNew(SBorder)
				.BorderImage( FAppStyle::GetBrush("Level.ColorIcon"))
				.BorderBackgroundColor(FLinearColor::Gray)
				[
					SAssignNew(ItemView, SW_ItemViewEntity, ItemAssetData)
				];
		}
		else
		{
			// Create hover widget
			return SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.ConnectorFeedback.Border"))
				.Content()
				[			
					SNew(SHorizontalBox)
				
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding( 0.0f, 0.0f, 3.0f, 0.0f )
					.VAlign( VAlign_Center )
					[
						SNew( SImage )
						.Image( this, &FDecoratedDragDropOp::GetIcon )
						.ColorAndOpacity( this, &FDecoratedDragDropOp::GetIconColorAndOpacity)
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign( VAlign_Center )
					[
						SNew(STextBlock) 
						.Text( this, &FDecoratedDragDropOp::GetHoverText )
					]
				];
		}
	}

protected:
	mutable TSharedPtr<SW_ItemViewEntity> ItemView;
};
