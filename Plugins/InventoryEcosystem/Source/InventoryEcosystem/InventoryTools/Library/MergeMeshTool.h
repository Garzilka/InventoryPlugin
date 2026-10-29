// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "InventoryEcosystem/InventoryTools/Data/MergeMeshData.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SkeletalMeshMerge.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "MergeMeshTool.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYTOOLS_API UMergeMeshTool : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static class USkeletalMesh* MergeMeshes(const FSkeletalMeshMergeParamsCustom& Params, bool WithMorph = false);

	static void OnAsyncLoadSkinComplete(TSoftObjectPtr<USkeletalMesh> Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent);

	static void AsyncLoadMaterialData(FSoftObjectPath Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent, const TCHAR* Pos);

	static void AsyncLoadTPSMeshData(FSoftObjectPath Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent);
};
