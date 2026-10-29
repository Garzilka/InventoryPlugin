#pragma once
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"

DECLARE_DELEGATE_OneParam(FSelectedItemSignature, class SW_ItemViewEntity*)

class SW_ItemViewEntity  : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ItemViewEntity) {}
		
	SLATE_EVENT( FSelectedItemSignature, OnClicked )
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, const TSharedPtr<FAssetData>& InAssetData);

	const FSlateBrush* GetItemIcon() const;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FReply Clicked();
	FSlateColor GetColor() const;
	TSharedPtr<FAssetData> GetAssetData() { return AssetData; }
	TWeakObjectPtr<UBaseItem> ItemRef;
private:
	TSharedPtr<SButton> CategoryBTN;
	TSharedPtr<FAssetData> AssetData;
	mutable FSlateBrush ItemBrush;
	
	FSelectedItemSignature OnClicked;
};
