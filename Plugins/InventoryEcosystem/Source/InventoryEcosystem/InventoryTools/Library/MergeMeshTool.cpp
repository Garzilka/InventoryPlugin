// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeMeshTool.h"


USkeletalMesh* UMergeMeshTool::MergeMeshes(const FSkeletalMeshMergeParamsCustom& Params, bool WithMorph)
{
	TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
	MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
		{
			return InMesh == nullptr;
		});

	if (MeshesToMergeCopy.Num() < 1) return nullptr;
	if (MeshesToMergeCopy.Num() == 1) return MeshesToMergeCopy[0];

	EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
		EMeshBufferAccess::ForceCPUAndGPU :
		EMeshBufferAccess::Default;
	TArray<FSkelMeshMergeSectionMapping> SectionMappings;
	TArray<FSkelMeshMergeUVTransforms> UvTransforms;

	USkeletalMesh* BakedMesh = NewObject<USkeletalMesh>();
	if (Params.Skeleton && Params.bSkeletonBefore)
	{
		BakedMesh->SetSkeleton(Params.Skeleton);
	}
	FSkeletalMeshMerge Merger(BakedMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
	if (!Merger.DoMerge())	return nullptr;

	if (Params.Skeleton && !Params.bSkeletonBefore)
	{
		BakedMesh->SetSkeleton(Params.Skeleton);
	}

	if (!WithMorph) return BakedMesh;

	int32 SectionShift = 0;
	int32 VerticesShift = 0;

	TArray<UMorphTarget*> ResultMorphs;
	for (int32 i = 0; i < MeshesToMergeCopy.Num(); i++)
	{
		TArray<UMorphTarget*> MorphTargetData = MeshesToMergeCopy[i]->GetMorphTargets();

		for (auto TargetMorph : MorphTargetData)
		{
			UMorphTarget* Dublicate = nullptr;
			for (auto FindDublicate : ResultMorphs)
			{
				if (FindDublicate->GetFName() == TargetMorph->GetFName())
				{
					Dublicate = FindDublicate;
					break;
				}
			}

			UMorphTarget* NewMorphTarget = Dublicate;

			if (!NewMorphTarget)
			{
				VerticesShift = 0;
				SectionShift = 0;

				NewMorphTarget = NewObject<UMorphTarget>(BakedMesh, TargetMorph->GetFName());

				for (int32 J = 0; J < i; J++)
				{
					for (int32 b = 0; b < MeshesToMergeCopy[J]->GetResourceForRendering()->LODRenderData[0].RenderSections.Num(); b++)
					{
						VerticesShift += MeshesToMergeCopy[J]->GetResourceForRendering()->LODRenderData[0].RenderSections[b].NumVertices;
						SectionShift++;
					}
				}
				for (FMorphTargetLODModel MorphTargetLODModel : TargetMorph->GetMorphLODModels())
				{
					for (int32& SectionId : MorphTargetLODModel.SectionIndices)
					{
						SectionId = (SectionId + ((SectionShift - 1) <= 0 ? SectionShift : SectionShift - 1));
					}

					for (auto& MorphTargetDelta : MorphTargetLODModel.Vertices)
					{
						MorphTargetDelta.SourceIdx += VerticesShift;
					}

					NewMorphTarget->GetMorphLODModels().Add(MorphTargetLODModel);
				}
				if (NewMorphTarget)
					ResultMorphs.Add(NewMorphTarget);
			}
			else
			{
				VerticesShift = 0;
				SectionShift = 0;
				for (int32 J = 0; J < i; J++)
				{
					for (int32 b = 0; b < MeshesToMergeCopy[J]->GetResourceForRendering()->LODRenderData[0].RenderSections.Num(); b++)
					{
						VerticesShift += MeshesToMergeCopy[J]->GetResourceForRendering()->LODRenderData[0].RenderSections[b].NumVertices;
						SectionShift++;
					}
				}
				for (FMorphTargetLODModel MorphTargetLODModel : TargetMorph->GetMorphLODModels())
				{
					for (int32& SectionId : MorphTargetLODModel.SectionIndices)
					{
						NewMorphTarget->GetMorphLODModels()[0].SectionIndices.Add((SectionId + ((SectionShift - 1) <= 0 ? SectionShift : SectionShift - 1)));
					}

					for (auto& MorphTargetDelta : MorphTargetLODModel.Vertices)
					{
						FMorphTargetDelta TargetDelta = MorphTargetDelta;
						TargetDelta.SourceIdx += VerticesShift;
						NewMorphTarget->GetMorphLODModels()[0].Vertices.Add(TargetDelta);
					}
					NewMorphTarget->GetMorphLODModels()[0].NumBaseMeshVerts = NewMorphTarget->GetMorphLODModels()[0].Vertices.Num();
				}
			}

		}
		VerticesShift = 0;
		SectionShift = 0;
	}

	for (auto MorphTarget : ResultMorphs)
	{
		BakedMesh->RegisterMorphTarget(MorphTarget, false);
	}

	BakedMesh->InitMorphTargets();
	BakedMesh->GetResourceForRendering()->ReleaseResources();
	BakedMesh->InitResources();

	return BakedMesh;
}



void UMergeMeshTool::OnAsyncLoadSkinComplete(TSoftObjectPtr<USkeletalMesh> Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent)
{
	if (!ArrayOwner) return;
	
	FMeshComponentData* AssetPtr = &ArrayOwner->Components[IdxComponent]; 
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();

	FSoftObjectPath MeshPath = Reference.ToSoftObjectPath();
	if (MeshPath.IsNull()) return;

	FMeshComponentData& LAssetPtr = *AssetPtr;
	if (!AssetLoader.IsAsyncLoadComplete(MeshPath))
	{
		AssetLoader.RequestAsyncLoad(MeshPath, FStreamableDelegate::CreateStatic(&UMergeMeshTool::AsyncLoadTPSMeshData, MeshPath, ArrayOwner, IdxComponent));
	}
	else
	{
		AssetLoader.LoadSynchronous(MeshPath);
		AssetPtr->MeshLoaded();
	}
};

void UMergeMeshTool::AsyncLoadTPSMeshData(FSoftObjectPath Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent)
{
	if (!ArrayOwner) return;
	if (!ArrayOwner->Components.IsValidIndex(IdxComponent)) return;

	USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(UObject::StaticClass(), nullptr, *(Reference.ToString())));
	if (!Mesh) return;

	for (int32 Idx = 0; Idx < ArrayOwner->Components.Num(); Idx++)
	{
		if (ArrayOwner->Components[Idx].MeshRef.Get() != Mesh) continue;
		ArrayOwner->Components[Idx].MeshLoaded();
		return;
	}
};

void UMergeMeshTool::AsyncLoadMaterialData(FSoftObjectPath Reference, FReplicatedMeshComponentsArray* ArrayOwner, int32 IdxComponent, const TCHAR* Pos)
{
	if (!ArrayOwner) return;
	if (!ArrayOwner->Components.IsValidIndex(IdxComponent)) return;
	ArrayOwner->Components[IdxComponent].SetMaterial(Pos, Cast<UMaterialInstance>(StaticLoadObject(UObject::StaticClass(), nullptr, *(Reference.ToString()))));
};