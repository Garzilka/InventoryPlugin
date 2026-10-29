// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"
#include "BaseWeaponModule.h"
#include "BaseWeaponScope.generated.h"


class USceneCaptureComponent2D;

UCLASS()
class HANDITEMSYSTEM_API ABaseWeaponScope : public ABaseWeaponModule
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UBaseItem>, FTransform> BaseOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UBaseItem>, FTransform> AIMOffset;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCapture;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool HasOptic = false;

public:	
	ABaseWeaponScope();

	UStaticMeshComponent* GetMesh() {return Mesh;};
	
	UFUNCTION(BlueprintCallable)
	void SetActiveRender(bool IsActive);
protected:
	virtual void BeginPlay() override;

};
