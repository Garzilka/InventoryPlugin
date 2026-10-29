// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DataAsset.h"
#include "BalisticFXData.generated.h"



USTRUCT(BlueprintType)
struct FDecalBalisticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AActor> BlueprintDecalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UMaterialInterface* BulitDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FVector SizeDecale = FVector(15.0f, 15.0f, 15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float LifeTime = 5.0f;
};

USTRUCT(BlueprintType)
struct FBalisticVFXData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setting)
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setting)
	TArray<UParticleSystem*> ImpactEmitter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setting)
	TArray<UNiagaraSystem*> ImpactNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setting)
	TArray<FDecalBalisticData> Decals;
};

UCLASS()
class HANDITEMSYSTEM_API UBalisticFXDataTable : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EPhysicalSurface>, FBalisticVFXData> BalisticData;

};
