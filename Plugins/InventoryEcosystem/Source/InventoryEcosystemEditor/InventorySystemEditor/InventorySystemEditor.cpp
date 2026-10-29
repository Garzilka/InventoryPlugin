// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventorySystemEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "ItemThumbnail/ItemThumbnail.h"

#define LOCTEXT_NAMESPACE "FInventorySystemEditorModule"

EAssetTypeCategories::Type FInventorySystemEditorModule::AssetCategory = static_cast<EAssetTypeCategories::Type>(0);

void FInventorySystemEditorModule::StartupModule()
{
    RegisterAssetCategory();
    UThumbnailManager::Get().RegisterCustomRenderer(UBaseItem::StaticClass(), UItemThumbnail::StaticClass());
}

void FInventorySystemEditorModule::ShutdownModule()
{
}

void FInventorySystemEditorModule::RegisterAssetCategory() const
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInventorySystemEditorModule, InventorySystemEditor)