// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipManagerTool.h"
#include "GameplayTagsManager.h"
#include "InventoryEcosystem/EquipManager/Public/EquipManagerSettings.h"


FGameplayTagContainer UEquipManagerTool::GetClothTags()
{
	return FGameplayTagContainer();
}

FGameplayTagContainer UEquipManagerTool::GetBodyTags()
{
	FGameplayTag Possible = (GetMutableDefault<UEquipManagerSettings>() != nullptr ? GetMutableDefault<UEquipManagerSettings>()->ContainerBody : FGameplayTag());
	if (!Possible.IsValid()) return FGameplayTagContainer();
	
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	return Manager.RequestGameplayTagChildren(Possible);
}
