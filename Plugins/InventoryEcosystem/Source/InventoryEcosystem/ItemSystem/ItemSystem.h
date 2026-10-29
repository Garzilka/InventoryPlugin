// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * Система предметов.
 * Знание о предметах: определения, экземпляры, сервисы, представление в мире, фабрики.
 * Об инвентарях (контейнерах) модуль ничего не знает.
 */
class FItemSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
