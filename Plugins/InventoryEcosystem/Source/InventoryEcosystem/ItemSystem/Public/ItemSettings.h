// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ItemSettings.generated.h"

/**
 * Настройки системы предметов.
 * Хранятся в Config/DefaultGame.ini, раздел [/Script/ItemSystem.ItemSettings].
 */
UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Система предметов"))
class ITEMSYSTEM_API UItemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Размер клетки предмета в интерфейсе (пиксели). Влияет только на отрисовку, не на геймплей. */
	UPROPERTY(Config, EditAnywhere, Category = "Система предметов", meta = (DisplayName = "Размер клетки", ClampMin = "4"))
	float ItemCellSize = 50.f;
};