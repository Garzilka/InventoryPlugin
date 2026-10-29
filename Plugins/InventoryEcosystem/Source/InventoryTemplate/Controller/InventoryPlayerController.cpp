// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPlayerController.h"


AInventoryPlayerController::AInventoryPlayerController()
{
	InventoryManager = CreateDefaultSubobject<UBaseInventoryManager>(TEXT("Inventory Manager"));
	LootManager = CreateDefaultSubobject<UBaseLootManager>(TEXT("Loot Manager"));
}
