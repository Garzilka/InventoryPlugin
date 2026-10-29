#include "HandItemSystemEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "PropertyEditorModule.h"
#include "ComboGraphEditor/ComboGraphAssetTypeActions.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FHandItemSystemEditorModule"

EAssetTypeCategories::Type FHandItemSystemEditorModule::AssetCategory = static_cast<EAssetTypeCategories::Type>(0);

#define LOCTEXT_NAMESPACE "ComboGraphEditor"
#define PLUGIN_NAME "InventoryEcosystem" 
#define CATEGORY_DISPLAY_NAME LOCTEXT(PLUGIN_NAME, "Combo Graph")

#define THUMBNAIL_PATH "ClassThumbnail.ComboGraphAsset"
#define ICON_PATH "ClassIcon.ComboGraphAsset"
#define ICON_NAME "ComboGraphIcon"
#define ICON_EXTENSION ".png"

void FHandItemSystemEditorModule::StartupModule()
{
    RegisterAssetCategory();
	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(PLUGIN_NAME, CATEGORY_DISPLAY_NAME);

	InitializeStyleSet();

	// ------------------------------
	// Register Combo Graph Asset
	// ------------------------------
	TSharedPtr<UComboGraphAssetTypeActions> ComboGraphAction = MakeShareable(new UComboGraphAssetTypeActions(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(ComboGraphAction.ToSharedRef());

	// ------------------------------
	// Register Combo Graph Pin Factory
	// ------------------------------
	PinFactory = MakeShareable(new FComboPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);

	// ------------------------------
	// Register Combo Graph Node Factory
	// ------------------------------
	NodeFactory = MakeShareable(new FComboNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("InventoryEcosystem")), FText::FromString(TEXT("InventoryEcosystem")));

	
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FHandItemSystemEditorModule::ShutdownModule()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
	}

	if (PinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	}

	if (NodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(NodeFactory);
	}
}

void FHandItemSystemEditorModule::RegisterAssetCategory() const
{
}

void FHandItemSystemEditorModule::InitializeStyleSet()
{
	StyleSet = MakeShareable(new FSlateStyleSet(TEXT(PLUGIN_NAME)));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PLUGIN_NAME);
	FString contentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(contentDir);

	FSlateImageBrush* IconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT(ICON_NAME), TEXT(ICON_EXTENSION)), FVector2D(150.0, 150.0));
	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT(ICON_NAME), TEXT(ICON_EXTENSION)), FVector2D(15.0, 15.0));
	StyleSet->Set(TEXT(THUMBNAIL_PATH), IconBrush);
	StyleSet->Set(TEXT(ICON_PATH), ThumbnailBrush);
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHandItemSystemEditorModule, HandItemSystemEditor)