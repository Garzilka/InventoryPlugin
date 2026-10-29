#include "W_ItemListView.h"

#include "Engine/AssetManager.h"
#include "Engine/Blueprint.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/DragAndDropOperations/Item/Item_EditorDragAndDrop.h"
#include "Widgets/Input/SSearchBox.h"


void SW_ItemListView::Construct(const FArguments& InArgs)
{
	ListView.Reset();
	
	TArray<FPrimaryAssetId> PrimaryAssetIds;
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.GetPrimaryAssetIdList(TEXT("TileItem"), PrimaryAssetIds);

	for (const FPrimaryAssetId& AssetId : PrimaryAssetIds)
	{
		FAssetData AssetData;
		if (AssetManager.GetPrimaryAssetData(AssetId, AssetData))
		{
			ListItems.Add(MakeShared<FAssetData>(AssetData));
		}
	}
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(12)
		[
			SNew(SSearchBox)
			.OnTextChanged(this, &SW_ItemListView::OnSearchBoxTextChanged)
		]
		+SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FAssetData>>)
			.ListItemsSource(&ListItems)
			.OnGenerateRow(this, &SW_ItemListView::GenerateRow)
		]
	];
	OnSearchBoxTextChanged(FText::FromString(""));
}

TSharedRef<ITableRow> SW_ItemListView::GenerateRow(TSharedPtr<FAssetData> AssetData,
	const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(STableRow<TSharedPtr<FAssetData>>, TableViewBase)
		.Padding(2.f)
		.OnDragDetected(this, &SW_ItemListView::OnRowDragDetected, AssetData)
   [
	  SNew(SW_ItemViewEntity, AssetData)
   ];
}

void SW_ItemListView::OnSearchBoxTextChanged(const FText& Text)
{
	ListItems.Empty();
	// 1. Получаем Asset Registry
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	// 2. Получаем имена ВСЕХ классов, которые наследуются от UBaseItem (включая C++ и загруженные БП)
	TSet<FTopLevelAssetPath> DerivedClassNames;
	TArray<FTopLevelAssetPath> BaseClassPaths { UBaseItem::StaticClass()->GetClassPathName() };
	AssetRegistry.GetDerivedClassNames(BaseClassPaths, TSet<FTopLevelAssetPath>(), DerivedClassNames);

	// 3. Настраиваем фильтр для поиска Блупринтов в проекте
	FARFilter Filter;
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;

	// Получаем список всех Блупринтов в проекте
	TArray<FAssetData> BlueprintAssets;
	AssetRegistry.GetAssets(Filter, BlueprintAssets);

	// 4. Фильтруем Блупринты: оставляем только те, чей класс есть в списке наследников DerivedClassNames
	for (const FAssetData& AssetData : BlueprintAssets)
	{
		if (AssetData.GetClass() == UBaseItem::StaticClass()) continue;
		
		// Извлекаем путь генерируемого класса из тегов ассета (без загрузки самого Блупринта)
		FAssetDataTagMapSharedView::FFindTagResult GeneratedClassTag = AssetData.TagsAndValues.FindTag(TEXT("GeneratedClass"));
		FString ClassPathStr = GeneratedClassTag.GetValue();
		FTopLevelAssetPath AssetClassPath(*FPackageName::ExportTextPathToObjectPath(ClassPathStr));

		if (DerivedClassNames.Contains(AssetClassPath))
		{
			if (Text.IsEmpty())
			{
				ListItems.Add(MakeShared<FAssetData>(AssetData));
				continue;
			}
			
			if (AssetData.AssetName.ToString().Contains(Text.ToString()))
			{
				ListItems.Add(MakeShared<FAssetData>(AssetData));
			}
		}
	}
	
	if (ListView.IsValid())
	{
		ListView->SetItemsSource(&ListItems);
		ListView->RebuildList();
	}
}

FReply SW_ItemListView::OnRowDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent,	TSharedPtr<FAssetData> AssetData) const
{
	if (PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		
		TSharedRef<STextBlock> DragVisual =
			SNew(STextBlock)
			.Text(FText::FromString("Tile Drag"));

		TSharedRef<FItem_EditorDragAndDrop> DragOp =
			FItem_EditorDragAndDrop::New(AssetData);
		

		return FReply::Handled().BeginDragDrop(DragOp);
	}

	return FReply::Unhandled();
}