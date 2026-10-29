#pragma once

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "ComboGraphEditor/Graph/Factory/ComboNodeFactory.h"
#include "ComboGraphEditor/Graph/Factory/ComboPinFactory.h"
#include "Modules/ModuleManager.h"

class FHandItemSystemEditorModule : public IModuleInterface
{
public:
    TSharedPtr<FComboPinFactory> PinFactory;
    TSharedPtr<FComboNodeFactory> NodeFactory;
    TSharedPtr<FSlateStyleSet> StyleSet;
    
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static EAssetTypeCategories::Type AssetCategory;
protected:
    void RegisterAssetCategory() const;
    void InitializeStyleSet();
    
};