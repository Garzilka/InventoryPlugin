// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeMeshData.h"
#include "InventoryEcosystem/InventoryTools/Component/MeshManager.h"
#include "InventoryEcosystem/InventoryTools/Library/MergeMeshTool.h"

void FMeshComponentData::MeshLoaded()
{
	if (MeshRef == TSoftObjectPtr<USkeletalMesh>())
	{
		if (WasLoaded() && Cast<UMeshManager>(ComponentCallBack))
			Cast<UMeshManager>(ComponentCallBack)->ComponentWasLoaded(this);
		return;
	};

	if (MeshRef.Get() == nullptr) return;

	if (WasLoaded() && Cast<UMeshManager>(ComponentCallBack))
		Cast<UMeshManager>(ComponentCallBack)->ComponentWasLoaded(this);
}

void FMeshComponentData::StartLoad(UActorComponent* Instigator, FReplicatedMeshComponentsArray& ArrayOwner, int32 IdxComponent)
{
	if (MeshRef == TSoftObjectPtr<USkeletalMesh>())
	{
		if (WasLoaded() && Cast<UMeshManager>(ComponentCallBack))
			Cast<UMeshManager>(ComponentCallBack)->ComponentWasLoaded(this);
		return;
	};
	ComponentCallBack = Instigator;
	UMergeMeshTool::OnAsyncLoadSkinComplete(MeshRef, &ArrayOwner, IdxComponent);
}