// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractLibrary.generated.h"


USTRUCT(BlueprintType)
struct FInteractDebugData
{
	GENERATED_BODY()

public:
	FColor BlockTraceColor;
	FColor TryTraceColor;
	bool IsEnabled = false;
	static FInteractDebugData MakeDefault(bool IsEnabled)
	{
		FInteractDebugData Result;
		Result.TryTraceColor = FColor::Green;
		Result.BlockTraceColor = FColor::Red;
		Result.IsEnabled = IsEnabled;
		return Result;
	}

	static FInteractDebugData MakeWallDetect(bool IsEnabled)
	{
		FInteractDebugData Result;
		Result.TryTraceColor = FColor::Blue;
		Result.BlockTraceColor = FColor::Red;
		Result.IsEnabled = IsEnabled;
		return Result;
	}

};

USTRUCT(BlueprintType)
struct FInteractTraceData
{
	GENERATED_BODY()

public:
	FName InteractProfile;
	float InteractionSphereSize;
	FVector CameraLocation;
	FVector EndVector;
	APawn* Pawn;
};

USTRUCT(BlueprintType)
struct FInteractOutputData
{
	GENERATED_BODY()

public:
	TArray<FHitResult> InOutHits;
};

UCLASS()
class INTERACTSYSTEM_API UInteractLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static bool CheckDotAngle(FVector From, class UBaseInteractComponent* LInteractComponent);
	static bool LineTrace(FInteractTraceData TraceData, TArray<FHitResult>& OutPut, FInteractDebugData IsShowDrawDebug);
	static bool PerformTrace(FInteractTraceData TraceData, TArray<FHitResult>& OutPut, FInteractDebugData IsShowDrawDebug);

	UFUNCTION(BlueprintCallable)
	static UInteractAnimationComponent* GetInteractAnimationComponent(UBaseInteractComponent* From);

	UFUNCTION(BlueprintCallable)
	static UInteractAnimationComponent* GetInteractAnimationFromActor(AActor* From);
};
