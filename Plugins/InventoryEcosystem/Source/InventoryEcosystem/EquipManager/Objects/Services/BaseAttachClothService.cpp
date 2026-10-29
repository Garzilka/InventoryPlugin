// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttachClothService.h"

#include "InventoryEcosystem/EquipManager/Data/CharacterMeshData.h"

FClothData UBaseAttachClothService::GetMeshData(FName Gender)
{
	FClothData* Ref = ClothGenderData.Find(Gender);
	if (Ref == nullptr)	return FClothData();
	return *Ref;
}
