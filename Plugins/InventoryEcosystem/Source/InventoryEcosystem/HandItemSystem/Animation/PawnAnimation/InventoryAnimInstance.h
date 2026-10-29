// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/HandItemSystem/Actor/WeaponSubsystem/Weapon/BaseFireWeapon.h"
#include "Animation/AnimInstance.h"
#include "InventoryAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HANDITEMSYSTEM_API UInventoryAnimInstance : public UAnimInstance
{
	GENERATED_BODY()



protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsFPS = false;
        USkeletalMeshComponent* GetMesh();
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll")
	FVector JoinTargetLocation;

	#pragma region Weapon
	UPROPERTY(BlueprintReadOnly)
	ABaseFireWeapon* WeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Кость правой руки"))
	FName RightHandBone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Точка прицеливания"))
	FName HeadBone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Точка прицеливания FPS"))
	FName HeadBoneLocaly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Сокет IK оружия"))
	FName LeftIkHandBone;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Скорость интерполяции"))
	float InterpSpeed = 3.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|AnimControll", meta = (DisplayName = "Скорость интерполяция от скорости"))
	float VelocityInterpSpeed = 3.2f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Aiming")
	FTransform FinalRelativeHand;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Aiming")
	FTransform CurrentSightSocketTransform;
    float CurrentDeltaSeconds = 0.f;

    UPROPERTY()
    USkeletalMeshComponent* _PRIVATE_Mesh = nullptr;
#pragma region Pose

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Позиция", meta = (DisplayName = "Текущий наклон"))
	float CurrentLean = 0.f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Позиция", meta = (DisplayName = "Трансформ IK левой руки"))
	FTransform LeftHandIKTransform;

#pragma endregion

#pragma region Recoil
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil", meta = (DisplayName = "Смещение прицеливания положения оружия"))
	FVector AIMPoseOffsetLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil", meta = (DisplayName = "Смещение прицеливания поворота оружия"))
	FRotator AIMPoseOffsetRotation;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil", meta = (DisplayName = "Базовое смещение положения оружия"))
	FVector BasePoseOffsetLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil", meta = (DisplayName = "Базовое смещение поворота оружия"))
	FRotator BasePoseOffsetRotation;

#pragma endregion

#pragma region Aiming

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Aiming")
	FTransform SightTransform;	
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil")
	FVector RecoilLocation;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Recoil")
	FRotator RecoilRotation;

	FTransform RecoilTransform;
	FTransform FinalRecoilTransform;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Sway")
	FVector SwayLocation;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Inventory|Weapon|Sway")
	FRotator SwayRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Weapon|Sway")
	float CharacterVelocity;

	float SwayMultiplier;
	float CurveTimer;
	float VelocityMultiplier;
	bool bInterpRecoil;
	float RecoilStartTime;
	float RecoilMultiplier;

#pragma endregion
#pragma endregion

	UPROPERTY(BlueprintReadOnly)
	ABaseCharacter* Character = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	UBaseHandItemManager* HandItemManager = nullptr;

	UPROPERTY()
	FVector2D Direction = FVector2D::ZeroVector;
	
	UPROPERTY()
	float Speed {0.f};
public:
	UFUNCTION(BlueprintPure, Meta = (BlueprintProtected, BlueprintThreadSafe))
	bool HasWeapon();

	UFUNCTION(BlueprintPure, Meta = (BlueprintProtected, BlueprintThreadSafe))
	bool IsLeftHandIK();
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION(BlueprintPure, meta = (BlueprintProtected, BlueprintThreadSafe, ReturnDisplayName = "Rig Input"))
	FVector2D GetDirection() {return Direction;};

	UFUNCTION(BlueprintPure, meta = (BlueprintProtected, BlueprintReadWrite))
	ABaseCharacter* GetPlayerCharacter();

	UFUNCTION()
	void NewWeapon(UBaseHandItemManager* HIM, AActor* NewActor, int32 Slot);

	UFUNCTION()
	void WeaponFire(ABaseFireWeapon* Weapon);

	UFUNCTION()
	void WeaponFireStatus(ABaseFireWeapon* Weapon, bool IsFire);

protected:
	
	void SetLeftIK();
	void WeaponSway(float DeltaSeconds);

	void RecoilInterpToZero(float DeltaSeconds);
	void RecoilInterpTo(float DeltaSeconds);
};
