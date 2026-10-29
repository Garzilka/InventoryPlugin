// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include "MoverTypes.h"
#include "MoverSimulationTypes.h"

#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"
#include "InventoryEcosystem/HandItemSystem/Data/RealisticDataModel.h"
#include "InventoryEcosystem/HandItemSystem/Interface/GetCharacterData.h"
#include "InventoryEcosystem/HandItemSystem/Objects/Ability/ContextAbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

class UBaseHandItemManager;
class UInventoryCameraComponent;
class URealisticMoverComponent;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemComponentInitialized, UAbilitySystemComponent*, ASC);

UCLASS()
class HANDITEMSYSTEM_API ABaseCharacter : public APawn, public IMoverInputProducerInterface, public IInventoryMeshInterface, public IGetCharacterData, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Ability System")
	FOnAbilitySystemComponentInitialized OnASCInitialized;
	
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "GAME|Components")
    TObjectPtr<UInventoryCameraComponent> CameraComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAME|Components")
    TObjectPtr<UCapsuleComponent> CapsuleComp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAME|Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	UContextAbilitySystemComponent* AbilitySystemComponent = nullptr;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAME|Components", meta = (AllowPrivateAccess = "true"))
    UBaseHandItemManager* HandItemManager;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление")
    UInputMappingContext* InputMapping;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> MoveAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> JumpAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> SprintAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> CrouchAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> LookMouseAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Action_AIM;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_01;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_02;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_03;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_04;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_05;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_06;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_07;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление|Слоты использования", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> UseSlot_08;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    TObjectPtr<UInputAction> Action_Melee;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    float LookUpMouseSensitivity{-1.f};
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Управление", Meta = (DisplayThumbnail = false))
    float LookRightMouseSensitivity{1.f};
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Настройки|Персонаж|Состояния", Transient)
    FInventoryViewState ViewState;
    
    UPROPERTY(EditAnywhere, Category = "GAME|Camera")
    float LookSensitivity = 1.0f;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GAME|Input")
    FVector CacheMoveInput = FVector::ZeroVector;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GAME|Input")
    ECharacterMovementType CurrentMovementType = ECharacterMovementType::ECMT_Idle;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRealisticMoverInputCmd Pawn_MoverInput;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMoverDataCollection MoverDataCollection;
    
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void OnJumpStarted(const FInputActionValue& Value);
    void OnSprintStarted(const FInputActionValue& Value);
    void OnSprintEnded(const FInputActionValue& Value);
    void OnCrouchStarted(const FInputActionValue& Value);
    void OnCrouchEnded(const FInputActionValue& Value);
	void Input_UseSlot(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_01(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_02(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_03(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_04(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_05(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_06(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_07(const FInputActionValue& ActionValue);
	virtual void Input_UseSlot_08(const FInputActionValue& ActionValue);
    
    bool bRequestJump = false;
    bool bRequestSprint = false;
    bool bRequestCrouch = false;

    FVector CameraTrace = FVector::ZeroVector;
    
    UPROPERTY()
	APlayerCameraManager* PlayerCameraManager;
private:
	bool bASCInitialized = false;
	
public:
	ABaseCharacter();
	
	UFUNCTION(BlueprintPure, Category = "Ability System")
	bool IsASCInitialized() const;
	
    APlayerCameraManager* GetCameraManager();
	
    UFUNCTION(BlueprintNativeEvent)
    FCharacterAnimationData I_GetAnimationData();
    FCharacterAnimationData I_GetAnimationData_Implementation();
    
    UBaseHandItemManager* GetHandItemManager() const {return HandItemManager;}
    
    virtual void Tick(float DeltaTime) override;
    
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // Генерация ввода для Mover
    UFUNCTION(BlueprintNativeEvent)
    void ProduceInput(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult);
    virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult);
    
    UFUNCTION(BlueprintCallable, Category = "Physics")
    void CheckFloorPhysics(float& OutFrictionMult);
    const FInventoryViewState& GetViewState() const {   return ViewState; };
    USkeletalMeshComponent* GetMesh() {return MeshComponent;}

    UFUNCTION(BlueprintNativeEvent)
    USkeletalMeshComponent* I_GetMesh();
    virtual USkeletalMeshComponent* I_GetMesh_Implementation() { return MeshComponent; };
    
    UFUNCTION(BlueprintNativeEvent)
    UCapsuleComponent* I_GetCapsuleComponent();
    virtual UCapsuleComponent* I_GetCapsuleComponent_Implementation() { return CapsuleComp; };
    
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginPlay() override;
	virtual void Input_OnMove(const FInputActionValue& ActionValue);
	virtual void Input_LookMouse(const FInputActionValue& ActionValue);
	virtual void Input_AIM(const FInputActionValue& ActionValue);
	virtual void Input_Melee(const FInputActionValue& ActionValue);
	virtual bool CanBeMove() {return true;}
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

private:
	UAbilitySystemComponent* InitAbilitySystemComponent();
	void RefreshGroundedRotation(float DeltaTime);
};
