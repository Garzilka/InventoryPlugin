// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "InventoryEcosystem/InventoryTools/Library/InventoryMathLib.h"

#include "InventoryEcosystem/HandItemSystem/Data/LocomotionDefines.h"
#include "InventoryEcosystem/HandItemSystem/Data/RealisticDataModel.h"
#include "InventoryEcosystem/HandItemSystem/Objects/MoveMods/RealisticAirMode.h"
#include "InventoryEcosystem/HandItemSystem/Objects/MoveMods/RealisticWalkMode.h"
#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/BaseHandItemManager.h"
#include "InventoryEcosystem/HandItemSystem/Components/InventoryCamera/InventoryCameraComponent.h"
#include "InventoryEcosystem/HandItemSystem/Components/MoveComponent/RealisticMoverComponent.h"
#include "InventoryEcosystem/HandItemSystem/Objects/Ability/ContextAbilitySystemComponent.h"


#pragma region ENGINE

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    HandItemManager = CreateDefaultSubobject<UBaseHandItemManager>(TEXT("Hand Item Manager"));
    CameraComponent = CreateDefaultSubobject<UInventoryCameraComponent>(TEXT("Camera Component"));
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UContextAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(34.0f, 88.0f);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Compon"));
    MeshComponent->SetupAttachment(CapsuleComp);
    MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;

    bReplicates = true;
    SetReplicateMovement(false);
}

bool ABaseCharacter::IsASCInitialized() const
{
	return bASCInitialized;
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
}

void ABaseCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
	InitAbilitySystemComponent();
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (IsLocallyControlled() && GetCameraManager())
    {
        FCollisionQueryParams LCQP;
        LCQP.AddIgnoredActor(this);
        
        FVector Start = GetCameraManager()->GetCameraLocation();
        FVector End = (Start + (GetCameraManager()->GetCameraRotation().Vector() * 1000.f));
        FHitResult OutHitResult;
        bool Result = GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility, LCQP);

        CameraTrace = (Result ? OutHitResult.Location : End);
    }
    RefreshGroundedRotation(DeltaTime);
}

#pragma endregion ENGINE 

#pragma region CAMERA

void ABaseCharacter::RefreshGroundedRotation(float DeltaTime)
{
    if (ViewState.CharacterAnimationData.MoveMode != ECharacterMoveMode::ECST_OnGround)
    {
        return;
    }

    // if (RotationMode == AlsRotationModeTags::ViewDirection)
    // {
    //     if ((!LocomotionState.bHasInput && LocomotionState.bRotationTowardsLastInputDirectionBlocked) ||
    //         !Settings->bAutoRotateOnAnyInputWhileNotMovingInViewDirectionRotationMode)
    //     {
    //         RefreshTargetYawAngleUsingActorRotation();
    //         return;
    //     }
    //
    //     // Rotate to the last view direction.
    //
    //     const auto TargetYawAngle{
    //         LocomotionState.bHasInput ? UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw) : LocomotionState.TargetYawAngle
    // };
    //
    //     const auto RotationInterpolationHalfLife{CalculateGroundedMovingRotationInterpolationHalfLife()};
    //
    //     static constexpr auto TargetYawAngleRotationSpeed{500.0f};
    //
    //     SetRotationExtraSmooth(TargetYawAngle, DeltaTime, RotationInterpolationHalfLife, TargetYawAngleRotationSpeed);
    //     
    //     LocomotionState.TargetYawAngle = FMath::UnwindDegrees(TargetYawAngle);
    //
    //     LocomotionState.SmoothTargetYawAngle = UAlsRotation::InterpolateAngleConstant(
    //             LocomotionState.SmoothTargetYawAngle, LocomotionState.TargetYawAngle, DeltaTime, RotationSpeed);
    //
    //     RefreshTargetYawAngleViewSpace();
    //
    //     auto NewRotation{GetActorRotation()};
    //     NewRotation.Yaw = UAlsRotation::DamperExactAngle(UE_REAL_TO_FLOAT(FMath::UnwindDegrees(NewRotation.Yaw)),
    //                                                      LocomotionState.SmoothTargetYawAngle, DeltaTime, InterpolationHalfLife);
    //
    //     SetActorRotation(NewRotation);
    //     return;
    // }
}

UAbilitySystemComponent* ABaseCharacter::InitAbilitySystemComponent()
{
    if(bASCInitialized || !IsValid(AbilitySystemComponent))
    {
        return nullptr;
    }
	
    bASCInitialized = true;
    AbilitySystemComponent->InitAbilityActorInfo(this, this); 

    OnASCInitialized.Broadcast(AbilitySystemComponent);
    return AbilitySystemComponent;
}

void ABaseCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo &ViewInfo)
{
    if (CameraComponent->IsActive())
    {
        CameraComponent->GetViewInfo(ViewInfo);
        return;
    }
    Super::CalcCamera(DeltaTime, ViewInfo);
}

#pragma endregion CAMERA

#pragma region INPUT

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;
    
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
    if (!Subsystem) return;
	
    Subsystem->ClearAllMappings();  
    Subsystem->AddMappingContext(InputMapping, 0);
    auto* EnhancedInput{ Cast<UEnhancedInputComponent>(PlayerInputComponent) };
    if (!IsValid(EnhancedInput)) return;
	
    if (MoveAction)
    {
    	// Вызывается каждый кадр, пока держишь кнопку
    	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
    	
    	// Вызывается ОДИН раз, когда ты отпускаешь кнопку. Value будет (0,0), и персонаж остановится.
    	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnMove);
    }

    if (LookMouseAction)
    {
    	EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
    }
    
    if (JumpAction)
    {
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseCharacter::OnJumpStarted);
    }
    
    if (SprintAction)
    {
    	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &ABaseCharacter::OnSprintStarted);
    	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABaseCharacter::OnSprintEnded);
    }
    
    if (CrouchAction)
    {
    	EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABaseCharacter::OnCrouchStarted);
    	EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABaseCharacter::OnCrouchEnded);
    }
    
    if (IsValid(Action_AIM))
    {
    	EnhancedInput->BindAction(Action_AIM, ETriggerEvent::Triggered, this, &ThisClass::Input_AIM);
    }
    
    if (IsValid(Action_Melee))
    {
    	EnhancedInput->BindAction(Action_Melee, ETriggerEvent::Triggered, this, &ThisClass::Input_Melee);
    }
    
    if (IsValid(UseSlot_01))
    {
        EnhancedInput->BindAction(UseSlot_01, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_01);
    }
    
    if (IsValid(UseSlot_02))
    {
        EnhancedInput->BindAction(UseSlot_02, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_02);
    }
    
    if (IsValid(UseSlot_03))
    {
        EnhancedInput->BindAction(UseSlot_03, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_03);
    }
    
    if (IsValid(UseSlot_04))
    {
        EnhancedInput->BindAction(UseSlot_04, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_04);
    }
    
    if (IsValid(UseSlot_05))
    {
        EnhancedInput->BindAction(UseSlot_05, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_05);
    }
    
    if (IsValid(UseSlot_06))
    {
        EnhancedInput->BindAction(UseSlot_06, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_06);
    }
    
    if (IsValid(UseSlot_07))
    {
        EnhancedInput->BindAction(UseSlot_07, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_07);
    }
    
    if (IsValid(UseSlot_08))
    {
        EnhancedInput->BindAction(UseSlot_08, ETriggerEvent::Triggered, this, &ThisClass::Input_UseSlot_08);
    }
}

// --- ГЛАВНАЯ ФУНКЦИЯ ВВОДА ---
void ABaseCharacter::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
    FRealisticMoverInputCmd& CharacterInputs = Pawn_MoverInput;
    InputCmdResult.InputCollection.AddDataByCopy(&Pawn_MoverInput);

    // Обработка физики поверхности (опционально, ваш старый код)
    float FrictionMult = 1.0f;
    CheckFloorPhysics(FrictionMult);

    FVector EffectiveInput = CacheMoveInput;
    if (FrictionMult < 0.5f)
    {
        EffectiveInput *= 0.2f; // Скользкий лед
    }

    // Заполняем данные
    CharacterInputs.ControlRotation = GetControlRotation();

    // В Mover 2.0 OrientationIntent и MoveInput разделены
    if (!EffectiveInput.IsNearlyZero())
    {
        CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, EffectiveInput);
        CharacterInputs.OrientationIntent = EffectiveInput; // Смотрим туда, куда идем
        CurrentMovementType = (bRequestSprint ? ECharacterMovementType::ECMT_Run : ECharacterMovementType::ECMT_Walk);
    }
    else
    {
        CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, FVector::ZeroVector);
        // Если стоим, не меняем ориентацию (или можно использовать ControlRotation.Vector())
        CharacterInputs.OrientationIntent = GetActorForwardVector();
        CurrentMovementType = ECharacterMovementType::ECMT_Idle;
    }

    CharacterInputs.bIsJumpPressed = bRequestJump;
    CharacterInputs.bIsCrouchPressed = bRequestCrouch;
    CharacterInputs.bIsSprintPressed = bRequestSprint;

    // Сбрасываем разовые триггеры
    bRequestJump = false;
}

void ABaseCharacter::Input_LookMouse(const FInputActionValue& ActionValue)
{
    const auto Value{ActionValue.Get<FVector2D>()};
    AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
    AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void ABaseCharacter::Input_UseSlot_01(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(0);
}

void ABaseCharacter::Input_UseSlot_02(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(1);
}

void ABaseCharacter::Input_UseSlot_03(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(2);
}

void ABaseCharacter::Input_UseSlot_04(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(3);
}

void ABaseCharacter::Input_UseSlot_05(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(4);
}

void ABaseCharacter::Input_UseSlot_06(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(5);
}

void ABaseCharacter::Input_UseSlot_07(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(6);
}

void ABaseCharacter::Input_UseSlot_08(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;
    HandItemManager->EquipItem(7);
}

void ABaseCharacter::Input_AIM(const FInputActionValue& ActionValue)
{
    const auto Value{ActionValue.Get<bool>()};
    if (Value)
    {
    	HandItemManager->SecondAction(true);
    }
    else
    {
    	HandItemManager->SecondAction(false);
    }
}

void ABaseCharacter::Input_Melee(const FInputActionValue& ActionValue)
{
    if (!HandItemManager) return;

    HandItemManager->MeleeAction();
}

void ABaseCharacter::Input_UseSlot(const FInputActionValue& ActionValue)
{
    // const auto Value{ActionValue.Get<int>()};
    if (!HandItemManager) return;
    // HandItemManager->EquipItem(Value);
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseCharacter::OnJumpStarted(const FInputActionValue& Value) { bRequestJump = true; }
void ABaseCharacter::OnSprintStarted(const FInputActionValue& Value)
{
    bRequestSprint = true;
    if (CurrentMovementType == ECharacterMovementType::ECMT_Idle) return;
    
    CurrentMovementType = (bRequestSprint ? ECharacterMovementType::ECMT_Run : ECharacterMovementType::ECMT_Walk);
}
void ABaseCharacter::OnSprintEnded(const FInputActionValue& Value) { bRequestSprint = false; }
void ABaseCharacter::OnCrouchStarted(const FInputActionValue& Value) { bRequestCrouch = true; }
void ABaseCharacter::OnCrouchEnded(const FInputActionValue& Value) { bRequestCrouch = false; }


void ABaseCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
    auto ViewRotation{GetViewState().Rotation};    
    const FVector2D InputVector{ UInventoryMathLib::ClampMagnitude012D(ActionValue.Get<FVector2D>()) };
    const FVector ForwardDirection{ UInventoryMathLib::AngleToDirectionXY(UE_REAL_TO_FLOAT(ViewRotation.Yaw)) };
    const FVector RightDirection{ UInventoryMathLib::PerpendicularCounterClockwiseXY(ForwardDirection) };
    CacheMoveInput = (RightDirection * InputVector.X) + (ForwardDirection * InputVector.Y);
    CacheMoveInput.Normalize();
}

#pragma endregion INPUT


#pragma region TOOLS

void ABaseCharacter::CheckFloorPhysics(float& OutFrictionMult)
{
    OutFrictionMult = 1.0f;
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0, 0, 100.0f);
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bReturnPhysicalMaterial = true;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        if (Hit.PhysMaterial.IsValid())
        {
            OutFrictionMult = Hit.PhysMaterial->Friction;
        }
    }
}


APlayerCameraManager * ABaseCharacter::GetCameraManager()
{
    if (IsValid(PlayerCameraManager)) return PlayerCameraManager;
    
    APlayerController* LPL = Cast<APlayerController>(GetController());
    if (!LPL) return PlayerCameraManager;

    PlayerCameraManager = LPL->PlayerCameraManager;
    return PlayerCameraManager;
}
#pragma endregion TOOLS




#pragma region IGetCharacterData

FCharacterAnimationData ABaseCharacter::I_GetAnimationData_Implementation()
{
    FCharacterAnimationData Result;
    Result.MovementType = CurrentMovementType;
    Result.StanceType = ECharacterStanceType::ECST_Stand;
    Result.Velocity = GetVelocity();
    Result.MoveInput = CacheMoveInput;
    Result.CameraTrace = CameraTrace;
    Result.CharacterTransform = GetTransform();
    Result.HasItemInHands = (GetHandItemManager() ? GetHandItemManager()->GetCurrentActor() != nullptr : false);
    
    return Result;
}


#pragma endregion IGetCharacterData
