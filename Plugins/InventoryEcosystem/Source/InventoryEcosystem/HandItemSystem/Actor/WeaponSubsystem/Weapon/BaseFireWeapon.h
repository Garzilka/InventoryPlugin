// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseWeapon.h" // IWYU pragma: keep
#include "InventoryEcosystem/HandItemSystem/Data/WeaponData.h"
#include "Animation/AnimInstance.h"
#include "BaseFireWeapon.generated.h"	

class UMeshManager;
class ABaseWeaponScope;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponFireSignature, class ABaseFireWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponFireStatusSignature, class ABaseFireWeapon*, Weapon, bool, IsFire);

UCLASS()
class HANDITEMSYSTEM_API ABaseFireWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireSignature OnWeaponFire;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireSignature OnChangeFireType;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireSignature OnChangeAmmo;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireSignature OnWeaponStartReload;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireSignature OnWeaponWasReload;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FWeaponFireStatusSignature OnWeaponFireStatus;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Базовые")
	UMeshManager* MeshManager;
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentUseRateDelay = 100.f;

	UPROPERTY(BlueprintReadOnly)
	USoundBase* CurrentFireSound = nullptr;

	UPROPERTY(Replicated)
	bool CanBeFire = false;

	float AimSwayMultiplier = 1.f;
	bool IsAiming = false;

	UPROPERTY(Replicated)
	EWeaponFireType CurrentFireType = EWeaponFireType::EWFT_None;

	UPROPERTY()
	bool HasTacticalAmmo = true;

	int32 NumShot = 0;

	UPROPERTY()
	FTransform CurrentIK_Offset;

	UPROPERTY()
	FVector JoinTarget;

	UPROPERTY()
	FWeaponSettingControll DefaultSettingControll;

	UPROPERTY()
	FWeaponSettingControll CurrentSettingControll;

	UPROPERTY()
	bool bIsFire = false;

	UPROPERTY()
	bool bIsReload = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsCharged = false;

	UPROPERTY()
	FTimerHandle Timer_AutoFire;

	UPROPERTY()
	FTimerHandle Timer_BurstFire;

	UPROPERTY()
	FTimerHandle Timer_StartWeaponReload;

	int32 BurstCounter = 0;

	UPROPERTY()
	FTimerHandle Timer_FireDelay;

	UPROPERTY(BlueprintReadOnly, Replicated)
	ABaseWeaponScope* CurrentOptic = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseWeaponScope*> Optics;

	UPROPERTY()
	class UBaseWeaponService* WeaponServiceCache = nullptr;

	UBaseWeaponService* GetWeaponService();
public:	
	ABaseFireWeapon();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	UMeshManager* GetMeshManager() const {return MeshManager;};
	
	UFUNCTION(BlueprintPure)
	FTransform GetLeftHandIK();
	
	UFUNCTION(BlueprintPure)
	FTransform GetBasePoseOffset(bool ForFPS);

	UFUNCTION(BlueprintPure)
	FTransform GetAIMPoseOffset(bool ForFPS);

	UFUNCTION(BlueprintPure)
	FTransform GetMovePoseOffset();

	UFUNCTION(BlueprintPure)
	FTransform GetHighPortPose();

	UFUNCTION(BlueprintPure)
	FTransform GetSightSocketTransform();

	UFUNCTION(BlueprintPure)
	bool IsUseIK();

	UFUNCTION(BlueprintPure)
	FVector GetJoinTargetLocation();

	UFUNCTION(BlueprintPure, Category = Weapon)
	FFirearmStats GetFirearmStats();

	void SetIsCharged(bool IsCharged = true);

	virtual bool IsReadyToUse() override;

	bool IsCharged();

	virtual UAnimInstance* GetAnimationInstace() override;

	virtual void InitWeapon(class UBaseWeaponData* InWeaponData) override;

	virtual bool I_SwitchScope_Implementation(bool Next);

	virtual bool Use(bool IsUse) override { return Fire(IsUse); }

	UFUNCTION(BlueprintCallable)
	bool Fire(bool IsUse);

	UFUNCTION(BlueprintCallable)
	bool BurstFire();

	UFUNCTION(BlueprintCallable)
	bool Reload();	

	UFUNCTION(BlueprintCallable)
	void NextFireType();	

	UFUNCTION(BlueprintPure, Category = Weapon)
	bool NeedToReload();

	UFUNCTION(BlueprintPure, Category = Weapon)
	EWeaponFireType GetCurrentFireType() { return CurrentFireType; };

	UFUNCTION(BlueprintPure, Category = Weapon)
	bool CanFire();

	UFUNCTION(BlueprintPure, Category = Weapon)
	bool IsReload() { return bIsReload; }

	UFUNCTION(BlueprintPure, Category = Weapon)
	FWeaponSettingControll GetCurrentSettingControll() { return CurrentSettingControll; }

	UFUNCTION(BlueprintPure, Category = Weapon)
	int32 GetMaxAmmo();

	UFUNCTION(BlueprintPure, Category = Weapon)
	int32 GetCurrentAmmo();

	UFUNCTION(BlueprintPure, Category = Weapon)
	UBaseFireWeaponData* GetFireWeaponData();

	UFUNCTION(BlueprintPure, Category = Weapon)
	FRecoilData GetRecoilData();

	UFUNCTION(BlueprintPure)
	float GetAimSwayMultiplier() const { return 1.f; }

	UFUNCTION(BlueprintPure, Category = Weapon)
	UCurveVector* GetMovementSwayCurve();
	bool UseStatsForMovementSway();
	virtual bool I_UnequipActor_Implementation() override;

	virtual bool Aim(bool IsUse) override;

	virtual bool AttachModule(UBaseWeaponModuleService* WeaponService, FWeaponSocketInfo Tag) override;

	virtual bool DetachModule(UBaseWeaponModuleService* WeaponService) override;
	virtual void BeginDestroy() override;
	virtual void RefreshAttachment() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_WeaponData() override;
	void SubstructAmmo(int32 Amount);
	
	void MagazineAnimation(FName PlayerSocket, FTransform Offset, bool ToPlayer);

	void WeaponWasReload();

	virtual bool Shooting();

	UFUNCTION(BlueprintNativeEvent)
	bool I_ReloadAction(bool IsReload);
	virtual bool I_ReloadAction_Implementation(bool IsReload) { return Reload(); }

	FRotator GetNextSpread();

	virtual bool SpawnProjectile(FRotator ProjectileRot);

	void ApplyRecoil(); 

	UFUNCTION(Reliable, Server)
	void Server_Reload();
	void Server_Reload_Implementation();

	UFUNCTION(Reliable, Server)
	void Server_SetNextFireType(EWeaponFireType FireType);
	void Server_SetNextFireType_Implementation(EWeaponFireType FireType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload();
	virtual void Multicast_Reload_Implementation() {}

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeFireState(bool State);
	virtual void Multicast_ChangeFireState_Implementation(bool State);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SingleShot(bool AdditionCharge);
	virtual void Multicast_SingleShot_Implementation(bool AdditionCharge) {}

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BirstFire();
	virtual void Multicast_BirstFire_Implementation();

	UFUNCTION()
	void LoopReload();
	UFUNCTION()
	void ExitReload();

	UAnimInstance* GetCharacterAnimInstance();

	virtual void OnRep_AttachedModules() override;
	virtual void RefreshRender() override;
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	UBaseFireWeaponData* CachedCastWeaponData = nullptr;
};
