// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/StreamableManager.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/NetSerialization.h"

#include "NativeGameplayTags.h"

#include "Net/Serialization/FastArraySerializer.h"

#include "Materials/MaterialInstance.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MergeMeshData.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentLoadCompleted, FFastArraySerializerItem, ComponentDataRef);

#pragma region MeshReplicate


USTRUCT(BlueprintType)
struct FMaterialInfo
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInstance> MaterialPtr = nullptr;

	UPROPERTY(NotReplicated)
	FSkeletalMaterial MaterialRef = FSkeletalMaterial();

};

USTRUCT(BlueprintType)
struct FMeshComponentData
{
	GENERATED_USTRUCT_BODY()


public:

	UPROPERTY()
	FGameplayTag BodyTag;

	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> MeshRef = TSoftObjectPtr<USkeletalMesh>();

	UPROPERTY()
	TArray<FMaterialInfo> MaterialMap;

	UPROPERTY(NotReplicated)
	UActorComponent* ComponentCallBack = nullptr;

	FMeshComponentData() {};

	USkeletalMesh* GetMesh() { return MeshRef.Get(); }

	bool WasLoaded()
	{
		if (MeshRef == TSoftObjectPtr<USkeletalMesh>()) return true;
		return (MeshRef.IsValid());
	}

	FMeshComponentData(TSoftObjectPtr<USkeletalMesh> InMeshRef, FGameplayTag InBodyTag)
	{
		MeshRef = InMeshRef;
		BodyTag = InBodyTag;
	};

	friend const bool operator==(const FMeshComponentData& Left, const FMeshComponentData& Right)
	{
		return Left.MeshRef == Right.MeshRef;
	};

	void SetMaterial(FString InKey, UMaterialInstance* Material) {};

	void InvalidPathMesh() {};

	void MeshLoaded();

	void StartLoad(UActorComponent* Instigator, FReplicatedMeshComponentsArray& ArrayOwner, int32 IdxComponent);
};

USTRUCT()
struct FReplicatedMeshComponentsArray
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FMeshComponentData> Components;

	using ElementType = FMeshComponentData;
	using ArrayType = TArray<ElementType>;
	FORCEINLINE ArrayType::RangedForConstIteratorType begin() const { return Components.begin(); }
	FORCEINLINE ArrayType::RangedForConstIteratorType end()   const { return Components.end(); }

	FMeshComponentData* Find(TSoftObjectPtr<USkeletalMesh> MeshRef)
	{
		for (int32 Idx = 0; Idx < Components.Num(); Idx++)
			if (Components[Idx].MeshRef == MeshRef)
				return &Components[Idx];
		return nullptr;
	}

	bool Add(FMeshComponentData InData)
	{
		if (InData.MeshRef == TSoftObjectPtr<USkeletalMesh>()) return false;
		FMeshComponentData* Component = Find(InData.MeshRef);
		if (Component) return true;
		Components.Add(InData);
		return true;
	}

	void Remove(FMeshComponentData InData)
	{
		for (int32 Idx = 0; Idx < Components.Num(); Idx++)
		{
			if (Components[Idx].MeshRef != InData.MeshRef) continue;
			Components.RemoveAt(Idx);
			return;
		}
	}

	bool StartLoad(UActorComponent* Instigator)
	{
		if (!Instigator) return false;
		if (Instigator->GetNetMode() == ENetMode::NM_DedicatedServer)	return true;

		for (int32 Idx = 0; Idx < Components.Num(); Idx++)
		{
			if (Components[Idx].WasLoaded()) continue;

			Components[Idx].StartLoad(Instigator, *this, Idx);
		}

		return true;
	}

	bool WasLoaded()
	{
		for (auto Component : Components)
		{
			if (!Component.WasLoaded()) return false;
		}

		return true;
	}
	bool IsNull()
	{
		return Components.Num() == 0;
	}
};

#pragma endregion


USTRUCT(BlueprintType)
struct INVENTORYTOOLS_API FSkelMeshMergeSectionMapping_BP
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
    TArray <int32> SectionIDs;
};

USTRUCT(BlueprintType)
struct INVENTORYTOOLS_API FSkelMeshMergeUVTransform
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
    TArray <FTransform> UVTransforms;
};

USTRUCT(BlueprintType)
struct INVENTORYTOOLS_API FSkelMeshMergeUVTransformMappingCustom
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
    TArray <FSkelMeshMergeUVTransform> UVTransformsPerMesh; 
};

USTRUCT(BlueprintType)
struct INVENTORYTOOLS_API FSkeletalMeshMergeParamsCustom
{
    GENERATED_BODY()

public:

	FSkeletalMeshMergeParamsCustom()
    {
        StripTopLODS = 0;
        bNeedsCpuAccess = false;
        bSkeletonBefore = false;
        Skeleton = nullptr;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkelMeshMergeSectionMapping_BP> MeshSectionMappings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkelMeshMergeUVTransformMappingCustom> UVTransformsPerMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<USkeletalMesh*> MeshesToMerge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StripTopLODS;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint32 bNeedsCpuAccess : 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint32 bSkeletonBefore : 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class USkeleton* Skeleton;
};
