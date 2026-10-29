// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItemService.h"
#include "InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h"
#include "BaseWorldItemService.generated.h"

UENUM(BlueprintType)
enum class EItemPlacementType : uint8
{
	EIPT_StaticMesh,
	EIPT_SkeletalMesh,
};

USTRUCT(BlueprintType)
struct FSpawnWorldItemServiceContext
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool WithSwitchParrent = true;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FTransform Transform;

public:
	FSpawnWorldItemServiceContext()
	{
		WithSwitchParrent = true;
	};

	FSpawnWorldItemServiceContext(FTransform InTransform)
	{
		WithSwitchParrent = true;
		Transform = InTransform;
	};

	FSpawnWorldItemServiceContext(bool InWithSwitchParrent, FTransform InTransform)
	{
		WithSwitchParrent = InWithSwitchParrent;
		Transform = InTransform;
	};

	bool IsWithSwitchParrent() { return WithSwitchParrent; };
	FTransform GetTransform() { return Transform; };
};

/*
* ������� ��� ������ �� �����
* ���������� - �����. ���������� ���� ������� ��� ������ �� ��������� (true) ��� ��� ����� �������� (false)
* @Item - ������� �� ��������
* @IsLeaveFromGrid - ������� ��� ������ �� �����(true) ��� ��� ����� ��������(false)
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnItemSignature, class UBaseWorldItemService*, Item, class ABaseItemActor*, InteractedItem);
/**
 * �� ���� ����, ���� ������ ������ �������� ������� � ���, ��� ��� ���� ����� ��������� �� ���������, ��� � ��� ���� ����� ������� ������� ��� ��� (HandItemManager)
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ITEMSYSTEM_API UBaseWorldItemService : public UBaseItemService
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FSpawnItemSignature OnPreSpawnItem;

	UPROPERTY()
	FSpawnItemSignature OnSpawnItem;

	UPROPERTY()
	FSpawnItemSignature OnMeshLoadAndSet;


protected:
	UPROPERTY(ReplicatedUsing = OnRep_SpawnedReference, DuplicateTransient)
	ABaseItemActor* SpawnedReference = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings")
	bool bIsMakeMeshComponent = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings")
	EItemPlacementType PlaceType = EItemPlacementType::EIPT_StaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings", meta = (EditCondition = "PlaceType == EItemPlacementType::EIPT_StaticMesh", EditConditionHides))
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings", meta = (EditCondition = "PlaceType == EItemPlacementType::EIPT_SkeletalMesh", EditConditionHides))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings")
	TMap<FName, TSoftObjectPtr<UMaterialInstance>> OverrideMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn settings")
	TSubclassOf<ABaseItemActor> CustomClass;

	UPROPERTY()
	UStaticMesh* CachedMesh = nullptr;

private:
	bool IsSpawnProcess = false;

public:
	
	UFUNCTION(BlueprintPure)
	bool IsMakeMeshComponent() const {return bIsMakeMeshComponent;}
	
	UFUNCTION(BlueprintPure)
	EItemPlacementType GetPlaceType() const {return PlaceType;}
	
	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<UStaticMesh> GetStaticMesh() {return StaticMesh;}
	
	UFUNCTION(BlueprintPure)
	TSoftObjectPtr<USkeletalMesh> GetSkeletalMesh() {return SkeletalMesh;}
	
	UFUNCTION(BlueprintPure)
	TSubclassOf<ABaseItemActor> GetClassSpawn() {return CustomClass;}
	
	UFUNCTION(BlueprintPure)
	TMap<FName, TSoftObjectPtr<UMaterialInstance>> GetOverrideMaterial() {return OverrideMaterial;}

	virtual void BeginPlay(UBaseItem* Item) override;

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetSpawnedRef() const { return SpawnedReference; }

	UFUNCTION(BlueprintPure)
	bool SetSpawnedRef(ABaseItemActor* NewReference);

	UFUNCTION(BlueprintCallable)
	ABaseItemActor* SpawnItem(FSpawnWorldItemServiceContext Context);

	UFUNCTION(BlueprintCallable)
	void DestroyFromWorld();

	template <class T>
	FORCEINLINE T* SpawnItemTyped(FTransform Position) { return Cast<T>(SpawnItem(Position)); };

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropToWorld(ACharacter* At);
	void Server_DropToWorld_Implementation(ACharacter* At);

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void DestroyActor(AActor* ItemActor);

	UFUNCTION()
	void OnRep_SpawnedReference();
};
