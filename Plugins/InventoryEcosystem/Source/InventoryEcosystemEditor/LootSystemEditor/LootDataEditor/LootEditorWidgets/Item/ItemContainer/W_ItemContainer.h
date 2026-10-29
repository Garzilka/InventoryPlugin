#pragma once
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "Widgets/SCompoundWidget.h"

class SW_ItemViewContainer : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ItemViewContainer)	
		: _PropertyHandle_Item(nullptr)			
	{}
		
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle_Item)
	SLATE_END_ARGS()

	
	void Construct(const FArguments& InArgs);
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
protected:
	void ValueChange();
	const FSlateBrush* GetItemIcon() const;
	UBaseItem* TileItemRef = nullptr;
	TSharedPtr<IPropertyHandle> PropertyHandle_Item;
	
	EVisibility GetVisibilityImage() const;
	mutable FSlateBrush ItemBrush;
};
