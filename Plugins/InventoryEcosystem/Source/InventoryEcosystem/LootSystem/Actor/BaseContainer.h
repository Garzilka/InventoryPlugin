// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"


#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseLineInventory.h"
#include "InventoryEcosystem/InventorySystem/Components/Inventory/BaseSpatialInventory.h"
#include "InventoryEcosystem/LootSystem/Component/LootManager/BaseLootComponent.h"
#include "InventoryEcosystem/LootSystem/Data/LootSystemData.h"


#include "BaseContainer.generated.h"

class AZoneLootManager;

USTRUCT(BlueprintType)
struct FContainerRespawnData
{
	GENERATED_BODY()

public:

    UPROPERTY()
    UBaseLineInventory* Inventory;
    UPROPERTY(NotReplicated)
    FDateTime TimeRespawn;
    FORCEINLINE friend bool operator==(FContainerRespawnData A, FContainerRespawnData B)
    {
    	return A.Inventory == B.Inventory;
    }
};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateContainerSignature, ABaseContainer*, ContainerRef);

UCLASS()
class LOOTSYSTEM_API ABaseContainer : public AActor
{
    GENERATED_BODY()

// public:	
//     UPROPERTY(BlueprintAssignable, Category = "Delegates")
//     FUpdateContainerSignature OnUpdateContainer;
	
protected:
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBaseLineInventory> ClassToInventory = UBaseSpatialInventory::StaticClass();
       
    
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBaseLootComponent* LootComponent = nullptr;
    
    UPROPERTY()
    USphereComponent* ContainerTrigger = nullptr;
    
    UPROPERTY(ReplicatedUsing = OnRep_Inventoryes)
    UBaseLineInventory* Inventory = nullptr;
    
    UPROPERTY(ReplicatedUsing = OnRep_Inventoryes)
    TArray<UBaseLineInventory*> UniqInventoryes;
    
    UPROPERTY()
    TArray<APlayerController*> RelewantFor;
    UPROPERTY()
    TArray<APawn*> Overlaped;
    UPROPERTY()
    TArray<APlayerController*> RelevanFor;
    UPROPERTY()
    FDateTime Date_RespawnLoot;
	
    UPROPERTY()
    FTimerHandle Timer_CheckInventory;
    
    UPROPERTY()
    TArray<AActor*> OverlappedActors; 
	
public:	
    ABaseContainer();
    virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure)
    UBaseLineInventory* GetInventoryByController(APlayerController* Controller);
protected:
    UBaseLineInventory* MakeInvetoryFor(APlayerController* Ref);
    UBaseLineInventory* MakeContainerInventory(bool WithCheck);
    UBaseLineInventory* FindOrAdd(APlayerController *Ref);
    void RemoveInventoryByController(AController* InOwner);

    UFUNCTION()
    void CheckInventory();
    
    void GenereteItems(APlayerController* For);	
    virtual void BeginPlay() override;
    void CheckInside();
    UFUNCTION()
    virtual void Sphere_BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void Sphere_EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
    UFUNCTION()
    void MakeSphereTrigger();
	
    UPROPERTY()
    FInventorySave SavedData;
	
    UFUNCTION()
    void AwaitPosses(TSoftObjectPtr<ACharacter> Character);
	
    UFUNCTION()
    void RunRespawn(int32 Id) {};

    void SetMultiOwner(TArray<APlayerController*> Owners, bool IsUniqueInventory) {};

    void AddRelevant(APlayerController* For) {};
	
    UFUNCTION()
    void RegisterLoginPlayer(AGameModeBase* GameMode, APlayerController* OutPlayer);
	
    UFUNCTION()
    void RegisterLogoutPlayer(AGameModeBase* GameMode, AController* OutPlayer);

    // virtual void BeginDestroy() override;
    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
    UFUNCTION()
    virtual void UniquePickuped(UBaseLineInventory* From, UBaseItem* Item, bool IsLeaveItem);

private:
    UFUNCTION()
    void OnRep_Inventoryes();
};
