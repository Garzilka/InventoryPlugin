// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshManager.h"


#pragma region ENGINE

UMeshManager::UMeshManager()
{
	SetIsReplicatedByDefault(true);
}

void UMeshManager::BeginPlay()
{
	Super::BeginPlay();
	BuildMesh();
}

#pragma endregion ENGINE

#pragma region REPLICATED

void UMeshManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UMeshManager, MeshComponents, COND_None, REPNOTIFY_Always);
}


void UMeshManager::OnRep_MeshRep()
{
	BuildMesh();
}

#pragma endregion REPLICATED

#pragma region ADD_REMOVE_CLEAR

void UMeshManager::AddMesh(FMeshComponentData Data)
{
	MeshComponents.Add(Data);
	BuildMesh();
}

void UMeshManager::RemoveMesh(FMeshComponentData Data)
{
	MeshComponents.Remove(Data);
	BuildMesh();
}

void UMeshManager::ClearMeshComponentData()
{
	MeshComponents.Components.Empty();
}

#pragma endregion

#pragma region BUILD

void UMeshManager::StartBuildMesh()
{
	if (MeshComponents.Components.Num() == 0) return;
	if (!MeshComponents.WasLoaded())
	{
		MeshComponents.StartLoad(this);
		return;
	}

	if (BaseSkeleton == nullptr && MeshComponents.Components[0].MeshRef.Get())
	{
		BaseSkeleton = MeshComponents.Components[0].MeshRef.Get()->GetSkeleton();
	}

	FSkeletalMeshMergeParamsCustom MeshMerge;

	MeshMerge.bSkeletonBefore = true;
	MeshMerge.Skeleton = BaseSkeleton;

	FReplicatedMeshComponentsArray LCharacterComponentsResult = FReplicatedMeshComponentsArray(MeshComponents);
	FReplicatedMeshComponentsArray LCharacterComponents;


	for (auto Component : MeshComponents)
	{
		if (!Component.GetMesh()) continue;
		MeshMerge.MeshesToMerge.Add(Component.GetMesh());
	}

	BackedMesh = UMergeMeshTool::MergeMeshes(MeshMerge, false);

	for (auto Component : MeshComponents)
	{
		for (auto Material : Component.MaterialMap)
		{
			int32 IdxTPS = GetMaterialIdx(BackedMesh, FName(Material.Slot));

			if (IdxTPS != INDEX_NONE)	BackedMesh->GetMaterials()[IdxTPS] = Material.MaterialPtr.LoadSynchronous();
		}
	}

	if (GetMesh() && BackedMesh) //Hasn't MeshComponent -> SetupTargetMesh from your actor
	{
		GetMesh()->SetSkeletalMesh(BackedMesh, false);
		GetMesh()->EmptyOverrideMaterials();
	}

	OnMeshMergeComplete.Broadcast(this);
}

bool UMeshManager::CheckLoad()
{
	if (!HasBegunPlay()) return false;
	if (!MeshComponents.WasLoaded())
	{
		MeshComponents.StartLoad(this);
		return false;
	}
	return true;
}

void UMeshManager::BuildMesh()
{
	if (!CheckLoad()) return;

	if (IsValid(this) && IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearTimer(RetriggerableDelay_BuildMesh);
		GetWorld()->GetTimerManager().SetTimer(RetriggerableDelay_BuildMesh, this, &UMeshManager::StartBuildMesh, 0.1f);
	}
}

void UMeshManager::ComponentWasLoaded(FMeshComponentData* ComponentLoaded)
{
	if (ComponentLoaded == nullptr) return;
	BuildMesh();
}

#pragma endregion BUILD

#pragma region TOOLS


int32 UMeshManager::GetMaterialIdx(USkeletalMesh* SkeletalMesh, FName MaterialSlotName)
{
	if (SkeletalMesh != nullptr)
	{
		const TArray<FSkeletalMaterial>& SkeletalMeshMaterials = SkeletalMesh->GetMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < SkeletalMeshMaterials.Num(); ++MaterialIndex)
		{
			const FSkeletalMaterial& SkeletalMaterial = SkeletalMeshMaterials[MaterialIndex];
			if (SkeletalMaterial.MaterialSlotName == MaterialSlotName)
			{
				return MaterialIndex;
			}
		}
	}
	return INDEX_NONE;
}

#pragma endregion TOOLS

