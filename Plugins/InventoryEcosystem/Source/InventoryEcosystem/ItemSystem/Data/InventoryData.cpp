// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryData.h"


namespace InventoryItemType
{
	UE_DEFINE_GAMEPLAY_TAG(None,		FName{ TEXTVIEW("IES.ItemType.None") });
	UE_DEFINE_GAMEPLAY_TAG(Muzzle,		FName{ TEXTVIEW("IES.ItemType.Weapon") });
	UE_DEFINE_GAMEPLAY_TAG(Receiver,	FName{ TEXTVIEW("IES.ItemType.Module") });
	UE_DEFINE_GAMEPLAY_TAG(FrontHandle, FName{ TEXTVIEW("IES.ItemType.Cloth") });
	UE_DEFINE_GAMEPLAY_TAG(Handle,		FName{ TEXTVIEW("IES.ItemType.Medicine") });
	UE_DEFINE_GAMEPLAY_TAG(Optic,		FName{ TEXTVIEW("IES.ItemType.Food") });
	UE_DEFINE_GAMEPLAY_TAG(Currency,	FName{ TEXTVIEW("IES.ItemType.Currency") });
}