// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"

#include "InventoryEcosystem/ItemSystem/Actor/BaseItemActor.h"
#include "InventoryEcosystem/HandItemSystem/Data/WeaponData.h"
#include "InventoryEcosystem/HandItemSystem/Interface/InventoryActorControll.h"

#include "BaseWeapon.generated.h"

UCLASS()
class HANDITEMSYSTEM_API ABaseWeapon : public ABaseItemActor, public IInventoryActorControll, public IInventoryMeshInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(ReplicatedUsing = OnRep_WeaponData, EditDefaultsOnly, BlueprintReadOnly, Category = "Базовые")
	class UBaseWeaponData* WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MeshComponent, meta = (AllowPrivateAccess = "true"))
	USceneComponent* RotateWepaon;

	UPROPERTY()
	class ACharacter* CharacterOwner;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttachedModules)
	TArray<FWeaponRepModData> Mods;
	
	
public:	
    ABaseWeapon();

    UFUNCTION(BlueprintNativeEvent)
    USkeletalMeshComponent* I_GetMesh();
    virtual USkeletalMeshComponent* I_GetMesh_Implementation() { return GetWeaponMesh(); };
    
    UFUNCTION(BlueprintNativeEvent)
    UCapsuleComponent* I_GetCapsuleComponent();
    virtual UCapsuleComponent* I_GetCapsuleComponent_Implementation() { return nullptr; };
	
    UFUNCTION(BlueprintPure)
    USkeletalMeshComponent* GetWeaponMesh();
	
    virtual void Tick(float DeltaTime) override;
    UFUNCTION(BlueprintPure)
    virtual UAnimInstance* GetAnimationInstace() { return nullptr;  };
    virtual bool IsReadyToUse() { return true; };
    
    UFUNCTION(BlueprintNativeEvent)
    bool I_EquipActor();
    virtual bool I_EquipActor_Implementation() { return false; }
    
    UFUNCTION(BlueprintNativeEvent)
    bool I_UnequipActor();
    virtual bool I_UnequipActor_Implementation() { return false; }
    
    UFUNCTION(BlueprintNativeEvent)
    bool I_FirstAction(bool IsUse);
    virtual bool I_FirstAction_Implementation(bool IsUse);
    
    UFUNCTION(BlueprintNativeEvent)
    bool I_SecondAction(bool IsUse);
    virtual bool I_SecondAction_Implementation(bool IsUse) { return Aim(IsUse); }
    
    UFUNCTION(BlueprintNativeEvent)
    bool I_SwitchScope(bool IsUse);
    virtual bool I_SwitchScope_Implementation(bool IsUse) { return false; }
    
    UFUNCTION(BlueprintCallable)
    virtual TArray<AActor*> GetModules();
    
    UFUNCTION(BlueprintCallable)
    virtual void InitWeapon(class UBaseWeaponData* InWeaponData);
    
    UFUNCTION(BlueprintCallable)
    virtual bool Use(bool IsUse) { return false; }
    
    UFUNCTION(BlueprintCallable)
    virtual bool Aim(bool IsUse) { return false; }
    
    virtual bool AttachModule(UBaseWeaponModuleService* WeaponService, FWeaponSocketInfo Tag) { return false; }
    
    virtual bool DetachModule(UBaseWeaponModuleService* WeaponService) { return false; }
    
    virtual void RefreshAttachment() {};
    
    UFUNCTION(BlueprintCallable)
    void AddCaptureRotate(FVector2D AddRot);
    
    UFUNCTION(BlueprintCallable)
    void ResetCaptureRotate();
    
    UFUNCTION(BlueprintCallable)
    void ActivateRenderTarget();
    
    UFUNCTION(BlueprintCallable)
    void DeactivateRenderTarget();
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	class ACharacter* GetCharacterOwner();

	UFUNCTION(Reliable, Server)
	void Server_Use(bool IsUse);
	virtual void Server_Use_Implementation(bool IsUse) { Use(IsUse); }

	bool IsLocallyControlled();

	UFUNCTION()
	virtual void OnRep_AttachedModules() { return; };

	UFUNCTION()
	virtual void OnRep_WeaponData();

	virtual void RefreshRender() { return; };

};
