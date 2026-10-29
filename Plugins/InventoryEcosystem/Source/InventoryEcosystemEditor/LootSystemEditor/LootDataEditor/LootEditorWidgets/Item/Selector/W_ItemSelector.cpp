#include "W_ItemSelector.h"
#include "DetailLayoutBuilder.h"
#include "EditorStyleSet.h"
#include "Engine/AssetManager.h"
#include "Engine/Blueprint.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "InventoryEcosystemEditor/LootSystemEditor/LootDataEditor/LootEditorWidgets/Item/Widget/W_ItemViewEntity.h"
#include "ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"


void SW_ItemSelectorMenu::Construct(const FArguments& InArgs)
{
	LootAssetEditor = InArgs._LootAssetEditorRef;
	OnClicked = InArgs._OnClicked;
	
	ChildSlot
	[
		SNew(SBox)
		.Padding(2.f)
		[
			SAssignNew(CB_MainMenu, SVerticalBox)
		]
	];
	
	CB_MainMenu->AddSlot()
	.AutoHeight()
	[
		SNew(SButton)
        .ButtonStyle( FCoreStyle::Get(), "NoBorder")
        .ClickMethod( EButtonClickMethod::MouseDown )
        .OnClicked( this, &SW_ItemSelectorMenu::Clicked )
        .ContentPadding(0.f)
        .ForegroundColor( FSlateColor::UseForeground() )
        .IsFocusable( false )
        [
        	SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
	        .AutoWidth()
			[
				SNew(SBox)
				.Padding(2.f, 0.f, 2.f, 0.f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text( FText::FromString("Clear"))
				]
			]
        ]
	];
	CB_MainMenu->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		.HeightOverride(0.5f)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
		]
	];
	CB_MainMenu->AddSlot()
	.AutoHeight()
	.Padding(12)
	[
		SNew(SSearchBox)
		.OnTextChanged(this, &SW_ItemSelectorMenu::OnSearchBoxTextChanged)
	];
	CB_MainMenu->AddSlot()
	.AutoHeight()
	[
		SNew(SBox)
		.HeightOverride(0.5f)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
		]
	];
	CB_MainMenu->AddSlot()
	.AutoHeight()
	.Padding(12)
	[
		SNew(SBox)
		.Padding(2.f)
		.MaxDesiredHeight(500.f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(CB_RowPicker, SVerticalBox)
			]
		]
		
	];
	OnSearchBoxTextChanged(FText::FromString(""));
	RebuildList();
}

void SW_ItemSelectorMenu::RebuildList()
{
	CB_RowPicker->ClearChildren();

	for (auto AssetData : ListItems)
	{
		CB_RowPicker->AddSlot()
		.AutoHeight()
		[
			SNew(SW_ItemViewEntity, AssetData)
			.OnClicked(this, &SW_ItemSelectorMenu::EventSelectedButtonCallback)
		];
	}
};


void SW_ItemSelectorMenu::EventSelectedButtonCallback(SW_ItemViewEntity* ObjectGUID)
{
	OnClicked.ExecuteIfBound(ObjectGUID);
}

void SW_ItemSelectorMenu::OnSearchBoxTextChanged(const FText& Text)
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

	RebuildList();
}

FReply SW_ItemSelectorMenu::Clicked()
{
	OnClicked.ExecuteIfBound(nullptr);
	return FReply::Handled();
}

void SW_ItemSelector::ValueChange()
{
	UObject* ValueObject = nullptr;
	if (PropertyHandle_Item->GetValue(ValueObject) == FPropertyAccess::Success)
	{
		UClass* ItemClass = Cast<UClass>(ValueObject);
		if (ItemClass)
		{
			TileItemRef = ItemClass->GetDefaultObject<UBaseItem>();
		}
		else
		{
			TileItemRef = nullptr;
		}
	}
}

void SW_ItemSelector::Construct(const FArguments& InArgs)
{
	LootAssetEditor = InArgs._LootAssetEditorRef;
	PropertyHandle_Item = InArgs._PropertyHandle_Item;
	
	SAssignNew(CM_Menu, SW_ItemSelectorMenu)
	.LootAssetEditorRef(LootAssetEditor)
	.OnClicked(this, &SW_ItemSelector::EventSelectedButtonCallback);
	
	FSimpleDelegate OnCategoryChange = FSimpleDelegate::CreateSP(this, &SW_ItemSelector::ValueChange);
	PropertyHandle_Item->SetOnPropertyValueChanged(OnCategoryChange);
	ValueChange();
	
	ChildSlot
	[
		SNew(SBox)	
		.Padding(2.f)
		[
			SNew(SBox)
			.MinDesiredWidth(120.f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SAssignNew(CB_Selector, SComboButton)
				.OnGetMenuContent(this, &SW_ItemSelector::OnGetKeyContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &SW_ItemSelector::GetCurrentKeyDesc)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			]
		]
	];
}

TSharedRef<SWidget> SW_ItemSelector::OnGetKeyContent() const
{
	return CM_Menu.ToSharedRef();
}

FText SW_ItemSelector::GetCurrentKeyDesc() const
{
	if (TileItemRef)
	{
		return FText::FromString(TileItemRef->GetClass()->GetName());
	}
	return FText::FromString(TEXT("None"));
}

void SW_ItemSelector::EventSelectedButtonCallback(SW_ItemViewEntity* ObjectGUID)
{
	CB_Selector->SetIsOpen(false);
	UObject* ValueObject = nullptr;
	if (PropertyHandle_Item->GetValue(ValueObject) == FPropertyAccess::Success)
	{
		PropertyHandle_Item->NotifyPreChange();
		TWeakObjectPtr<UBaseItem> InAssetData = (ObjectGUID != nullptr ?  ObjectGUID->ItemRef : nullptr);
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
}



