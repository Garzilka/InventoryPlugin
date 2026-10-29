#include "InventoryCoreEditor.h"

void FInventoryCoreEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FInventoryCoreEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

IMPLEMENT_MODULE(FInventoryCoreEditorModule, InventoryCoreEditor)