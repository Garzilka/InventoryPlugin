// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"

class FInventorySystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static EAssetTypeCategories::Type AssetCategory;

protected:
	void RegisterAssetCategory() const;
};
