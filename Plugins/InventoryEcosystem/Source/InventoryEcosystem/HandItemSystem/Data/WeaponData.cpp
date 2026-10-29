// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponData.h"


namespace WeaponModificationType
{
	UE_DEFINE_GAMEPLAY_TAG(None,		FName{ TEXTVIEW("IES.WeaponModuleType.None") });
	UE_DEFINE_GAMEPLAY_TAG(Muzzle,		FName{ TEXTVIEW("IES.WeaponModuleType.Muzzle") });
	UE_DEFINE_GAMEPLAY_TAG(Receiver,	FName{ TEXTVIEW("IES.WeaponModuleType.Receiver") });
	UE_DEFINE_GAMEPLAY_TAG(FrontHandle, FName{ TEXTVIEW("IES.WeaponModuleType.FrontHandle") });
	UE_DEFINE_GAMEPLAY_TAG(Handle,		FName{ TEXTVIEW("IES.WeaponModuleType.Handle") });
	UE_DEFINE_GAMEPLAY_TAG(Optic,		FName{ TEXTVIEW("IES.WeaponModuleType.Optic") });
	UE_DEFINE_GAMEPLAY_TAG(Buttstock,	FName{ TEXTVIEW("IES.WeaponModuleType.Buttstock") });
	UE_DEFINE_GAMEPLAY_TAG(Magazine,	FName{ TEXTVIEW("IES.WeaponModuleType.Magazine") });
	UE_DEFINE_GAMEPLAY_TAG(Tactical,	FName{ TEXTVIEW("IES.WeaponModuleType.Tactical") });
	UE_DEFINE_GAMEPLAY_TAG(Forearm,		FName{ TEXTVIEW("IES.WeaponModuleType.Forearm") });
	UE_DEFINE_GAMEPLAY_TAG(Additional,	FName{ TEXTVIEW("IES.WeaponModuleType.Additional") });
	UE_DEFINE_GAMEPLAY_TAG(MuzzleAdd,	FName{ TEXTVIEW("IES.WeaponModuleType.MuzzleAdd") });
}