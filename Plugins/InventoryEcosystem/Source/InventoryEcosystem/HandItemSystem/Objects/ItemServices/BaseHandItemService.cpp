// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHandItemService.h"

bool UBaseHandItemService::GetHandData(FGameplayTag Tag, FEquipAnimation& Result)
{
	auto ResultData = EquipData.Find(Tag);
	if (!ResultData) return false;
	Result = *ResultData;
	return true;
}

void UBaseHandItemService::PlayEquipAnimation(FEquipAnimation Data, bool IsEquip)
{
	OnHandItemObjectPlayAnimation.Broadcast(this, IsEquip);
}
