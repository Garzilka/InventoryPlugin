#pragma once
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SW_ItemListView : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SW_ItemListView){}
	SLATE_END_ARGS()

	
	void Construct(const FArguments& InArgs);
	void OnSearchBoxTextChanged(const FText& Text) ;


	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FAssetData> AssetData, const TSharedRef<STableViewBase>& TableViewBase);
	FReply OnRowDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent, TSharedPtr<FAssetData> AssetData) const;

protected:
	TArray<TSharedPtr<FAssetData>> ListItems;
	TSharedPtr<class SListView<TSharedPtr<FAssetData>>> ListView;
	
};
