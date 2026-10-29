// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryTools/Component/MeshManager.h"
#include "InventoryEcosystem/EquipManager/Data/CharacterMeshData.h"
#include "CharacterMeshManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EQUIPMANAGER_API UCharacterMeshManager : public UMeshManager
{
	GENERATED_BODY()

private:
	bool bIsFPSMode;

	UPROPERTY(EditDefaultsOnly, Category = "Настройки|MeshManager|FPS", meta = (DisplayName = "TPS меши в FPS режиме"))
	FGameplayTagContainer ViewInFPS;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Настройки|MeshManager|FPS", meta = (AllowPrivateAccess = "true", DisplayName = "Старт в режиме FPS", EditCondition = bEnableFPS, EditConditionHides))
	bool bIsStartInFPSMode = true;
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ViewMesh;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShadowMesh;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* BackedFPSMesh;
	
public:	

	UFUNCTION(BlueprintPure, meta = (ToolTip = "Return true if FPS mode and return always false if not locally"))
	const bool& IsFPSMode() const { return bIsFPSMode; }

	UFUNCTION(BlueprintCallable)
	virtual void SetFPSMode(bool InIsFPS);
	
	UFUNCTION(BlueprintCallable)
	virtual void SwitchViewPlayer(bool IsView) {};

	UFUNCTION(BlueprintPure)
	const USkeletalMeshComponent* GetShadowMesh() const { return ShadowMesh; }

	UFUNCTION(BlueprintPure)
	USkeletalMesh* GetBackedFPSMesh() const { return BackedFPSMesh; }

	UFUNCTION(BlueprintPure)
	USkeletalMesh* GetBackedFullBodyMesh() const { return BackedMesh; }
	
protected:
	virtual void BeginPlay() override;
	virtual void StartBuildMesh() override;

		
};
