// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryCoreLibrary.generated.h"

class IItemManagmentInterface;
class IInventoryMeshInterface;
class UCoreInventoryObject;

UENUM()
enum class E_IES_LOG_TYPE : uint8
{
	EIEST_Display,
	EIEST_Warning,
	EIEST_Error
};


UCLASS()
class INVENTORYCORE_API UInventoryCoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static IItemManagmentInterface* HasManagmentInterface(UObject* TargetObject);
	static IInventoryMeshInterface* HasMeshInterface(UObject* TargetObject);

	static USkeletalMeshComponent* GetInventoryMesh(AActor* From);

	static void PrintBIO_LOG(AActor* Owner, UCoreInventoryObject* InObject, E_IES_LOG_TYPE TypeLog, FString Str, int Line, const char* Function);
	
	UFUNCTION()
	static void PrintString(FString Message, FColor Color = FColor::Blue);
	static FString GetPlayerNickname(AActor* From);
	static APlayerState* GetPlayerState(AActor* Owner);
	static APlayerState* GetPlayerState(ACharacter* Owner);
	static APlayerState* GetPlayerState(APlayerController* Owner);
	static FColor ErrorToColor(E_IES_LOG_TYPE ErrorType);
	static FString GetNetMode(AActor* InActor);
	
};
