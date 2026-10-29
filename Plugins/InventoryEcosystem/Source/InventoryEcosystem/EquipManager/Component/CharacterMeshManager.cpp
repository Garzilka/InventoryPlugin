// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMeshManager.h"

#include "Components/SkeletalMeshComponent.h"

#pragma region Engine

void UCharacterMeshManager::BeginPlay()
{
	bIsFPSMode = bIsStartInFPSMode;

	ViewMesh = NewObject<USkeletalMeshComponent>(GetOwner());
	ViewMesh->SetupAttachment(GetMesh());
	ViewMesh->SetLeaderPoseComponent(GetMesh());
	ViewMesh->RegisterComponentWithWorld(GetWorld());
	ViewMesh->SetCastShadow(false);

	ShadowMesh = NewObject<USkeletalMeshComponent>(GetOwner());
	ShadowMesh->SetupAttachment(GetMesh());
	ShadowMesh->SetLeaderPoseComponent(GetMesh());
	ShadowMesh->RegisterComponentWithWorld(GetWorld());
	ShadowMesh->bOwnerNoSee = true;
	ShadowMesh->SetHiddenInGame(true);
	ShadowMesh->bCastHiddenShadow = true;

	Super::BeginPlay();
}
#pragma endregion //Engine


#pragma region BUILD

void UCharacterMeshManager::StartBuildMesh()
{
	if (MeshComponents.Components.Num() == 0) return;
	if (!CheckLoad()) return;

	if (BaseSkeleton == nullptr && MeshComponents.Components[0].MeshRef.Get())
	{
		BaseSkeleton = MeshComponents.Components[0].MeshRef.Get()->GetSkeleton();
	}

	FSkeletalMeshMergeParamsCustom TPSMeshMerge;
	FSkeletalMeshMergeParamsCustom FPSMeshMerge;

	TPSMeshMerge.bSkeletonBefore = true;
	TPSMeshMerge.Skeleton = BaseSkeleton;

	FPSMeshMerge.bSkeletonBefore = true;
	FPSMeshMerge.Skeleton = BaseSkeleton;
	FReplicatedMeshComponentsArray LCharacterComponentsResult = FReplicatedMeshComponentsArray(MeshComponents);

	for (auto Component : LCharacterComponentsResult)
	{
		if (!Component.GetMesh()) continue;
		TPSMeshMerge.MeshesToMerge.Add(Component.GetMesh());

		if (ViewInFPS.HasTag(Component.BodyTag))
			FPSMeshMerge.MeshesToMerge.Add(Component.GetMesh()); //TPS meshes in fps mode
	}

	BackedMesh = UMergeMeshTool::MergeMeshes(TPSMeshMerge, false);
	BackedFPSMesh = UMergeMeshTool::MergeMeshes(FPSMeshMerge, false);

	if (bDoNeedPhysicOverride)
	{
		if(BackedMesh)
			BackedMesh->SetPhysicsAsset(PhysicAsset);

		if (BackedFPSMesh)
			BackedFPSMesh->SetPhysicsAsset(PhysicAsset);
	}

	for (auto Component : LCharacterComponentsResult)
	{
		for (auto Material : Component.MaterialMap)
		{
			int32 IdxTPS = GetMaterialIdx(BackedMesh, FName(Material.Slot));
			int32 IdxFPS = GetMaterialIdx(BackedFPSMesh, FName(Material.Slot));

			if (IdxTPS != INDEX_NONE)	BackedMesh->GetMaterials()[IdxTPS] = Material.MaterialPtr.LoadSynchronous();
			if (IdxFPS != INDEX_NONE)	BackedFPSMesh->GetMaterials()[IdxFPS] = Material.MaterialPtr.LoadSynchronous();
		}
	}

	if (GetMesh())
	{
		GetMesh()->SetSkeletalMesh((IsFPSMode() ? BackedFPSMesh : BackedMesh), false);
		GetMesh()->EmptyOverrideMaterials();
	}

	ShadowMesh->SetSkeletalMesh(BackedMesh);
	OnMeshMergeComplete.Broadcast(this);
}


#pragma endregion BUILD 


#pragma region TOOLS

void UCharacterMeshManager::SetFPSMode(bool InIsFPS)
{
	bIsFPSMode = InIsFPS;
	if (!GetMesh()) return;
	GetMesh()->SetSkeletalMesh((IsFPSMode() ? BackedFPSMesh : BackedMesh), false);
}

#pragma endregion TOOLS