// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreComponent.h"
#include "NativeGameplayTags.h"

#include "Net/UnrealNetwork.h"	
#include "Engine/ActorChannel.h"

#include "InventoryEcosystem/InventoryTools/Data/MergeMeshData.h"
#include "InventoryEcosystem/InventoryTools/Library/MergeMeshTool.h"
#include "MeshManager.generated.h"

/** Меш собран */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMeshMergeCompleteSignature, class UMeshManager*, MeshManagerRef);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYTOOLS_API UMeshManager : public UInventoryCoreComponent
{
	GENERATED_BODY()


public:
	/** Вызовется когда меш соберется */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FMeshMergeCompleteSignature OnMeshMergeComplete;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки|MeshManager|Mesh", meta = (DisplayName = "Перезаписать Physics Asset", ToolTip = "Необходимо ли перезаписывать Physics Asset для скелетов?", EditCondition = bEnableFPS, EditConditionHides))
	bool bDoNeedPhysicOverride = true;

	UPROPERTY(EditDefaultsOnly, Category = "Настройки|MeshManager|Mesh", meta = (EditCondition = bDoNeedPhysicOverride, EditConditionHides))
	UPhysicsAsset* PhysicAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Настройки|MeshManager|Mesh")
	USkeleton* BaseSkeleton;

	UPROPERTY()
	USkeletalMesh* BackedMesh;

	UPROPERTY(ReplicatedUsing = OnRep_MeshRep)
	FReplicatedMeshComponentsArray MeshComponents;

	UPROPERTY()
	FTimerHandle RetriggerableDelay_BuildMesh;
	
public:	
	UMeshManager();

	UFUNCTION(BlueprintPure)
	USkeletalMesh* GetMergedMesh() { return BackedMesh; }

	UFUNCTION(BlueprintPure)
	int32 GetMaterialIdx(USkeletalMesh* SkeletalMesh, FName MaterialSlotName);

	UFUNCTION(BlueprintCallable)
	void BuildMesh();

	void ComponentWasLoaded(FMeshComponentData* ComponentLoaded);

	UFUNCTION(BlueprintPure)
	bool IsNeedOverridePhysic() { return bDoNeedPhysicOverride; }

	UFUNCTION(BlueprintPure)
	UPhysicsAsset* GetPhysicAssetOverride() { return PhysicAsset; }

	UFUNCTION(BlueprintCallable)
	void AddMesh(FMeshComponentData Data);

	UFUNCTION(BlueprintCallable)
	void RemoveMesh(FMeshComponentData Data);

	UFUNCTION(BlueprintCallable)
	void ClearMeshComponentData();

protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void StartBuildMesh();

	UFUNCTION()
	bool CheckLoad();

	UFUNCTION()
	void OnRep_MeshRep();

		
};
