// Copyright Epic Games, Inc. All Rights Reserved.

#include "LootSystemEditor.h"

#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"
#include "InventoryEcosystem/InteractSystem/Data/InteractData.h"
#include "InventoryEcosystem/LootSystem/Data/CurrencyData.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"
#include "LootDataEditor/Factoryes/LootEditor_AssetFactory.h"
#include "TradeDataEditor/CustomFuncPinTrade/CustomPinFuncTrade.h"
#include "TradeDataEditor/CustomizationCurrencyType/FCustomizationCurrencyType.h"
#include "TradeDataEditor/CustomizationCurrencyValue/FCustomizationCurrencyValue.h"
#include "TradeDataEditor/CustomizationItemCurrenciesData/FCustomizationItemCurrenciesData.h"
#include "TradeDataEditor/CustomizationTraderMenu/CustomizationTraderMenu.h"
#include "TradeDataEditor/CustomizationTradeType/FCustomizationTradeType.h"

#define LOCTEXT_NAMESPACE "FLootSystemEditorModule"

EAssetTypeCategories::Type  FLootSystemEditorModule::InventoryEcosystemCategory = static_cast<EAssetTypeCategories::Type>(0);

void FLootSystemEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    InventoryEcosystemCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("InventoryEcosystem")), FText::FromString(TEXT("InventoryEcosystem")));
	
	TSharedPtr<FCustomPinFuncTrade> Factory = MakeShareable(new FCustomPinFuncTrade());
	FEdGraphUtilities::RegisterVisualPinFactory(Factory);
	
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_LootEditor_AssetAction()));
	PropertyModule.RegisterCustomPropertyTypeLayout(FName(FCurrencyType::StaticStruct()->GetName()), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomizationCurrencyType::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FName(FTradeType::StaticStruct()->GetName()), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomizationTradeType::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FName(FCurrencyValue::StaticStruct()->GetName()), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomizationCurrencyValue::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FName(FItemCurrecyTradeData::StaticStruct()->GetName()), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomizationItemCurrenciesData::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(FName(FCurrencyTraderData::StaticStruct()->GetName()), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomizationTraderMenu::MakeInstance));
    		
    PropertyModule.NotifyCustomizationModuleChanged();
}

void FLootSystemEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLootSystemEditorModule, LootSystemEditor)