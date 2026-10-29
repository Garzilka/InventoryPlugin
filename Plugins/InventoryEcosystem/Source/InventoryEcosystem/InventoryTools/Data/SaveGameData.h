// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive, const bool bSaveGame = true) : FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = bSaveGame; //Consider only 'Save Game' variables
		ArNoDelta = true;  //Allow to save default values
	}
};

class INVENTORYTOOLS_API SaveGameData
{
public:
	SaveGameData();
	~SaveGameData();
};
