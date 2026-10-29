#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectMacros.h"
#include "LocomotionDefines.generated.h"

UENUM(BlueprintType)
enum class EMoveSwichAnimType : uint8
{
    EMSAT_IdleLoop                      UMETA(DisplayName = "Idle Loop"),
    EMSAT_TransitionToIdleLoop          UMETA(DisplayName = "Transition To Idle Loop"),
    EMSAT_LocomotionLoop                UMETA(DisplayName = "Locomotion Loop"),
    EMSAT_TransitionToLocomotionLoop    UMETA(DisplayName = "Transition to Locomotion Loop"),
    EMSAT_InAirLoop                     UMETA(DisplayName = "In Air Loop"),
    EMSAT_TransitionToInAirLoop         UMETA(DisplayName = "Transition to In Air Loop"),
    EMSAT_IdleBreak                     UMETA(DisplayName = "Idle Break"),
    EMSAT_TransitionToSlide             UMETA(DisplayName = "Transition to Slide"),
    EMSAT_SlideLoop                     UMETA(DisplayName = "Slide Loop"),
};

UENUM(BlueprintType)
enum class ECharacterMoveMode : uint8
{
    ECST_OnGround       UMETA(DisplayName = "Stand"),
    ECST_InAir          UMETA(DisplayName = "Crouch"),
    ECST_Traversing     UMETA(DisplayName = "Prone"),
    ECST_Sliding        UMETA(DisplayName = "Sliding"),
};

UENUM(BlueprintType)
enum class ECharacterStanceType : uint8
{
    ECST_Stand      UMETA(DisplayName = "Stand"),
    ECST_Crouch     UMETA(DisplayName = "Crouch"),
    ECST_Prone      UMETA(DisplayName = "Prone"),
    ECST_Swiming    UMETA(DisplayName = "Swiming"),
};

UENUM(BlueprintType)
enum class ECharacterMovementType : uint8
{
    ECMT_Idle       UMETA(DisplayName = "Idle"),
    ECMT_Walk       UMETA(DisplayName = "Walk"),
    ECMT_Run        UMETA(DisplayName = "Run"),
    ECMT_Sprint     UMETA(DisplayName = "Sprint"),
};

UENUM(BlueprintType)
enum class ECharacterViewType : uint8
{
    ECVT_FPS       UMETA(DisplayName = "FPS"),
    ECVT_TPS       UMETA(DisplayName = "TPS"),
    ECVT_TPSFree        UMETA(DisplayName = "TPS Free"),
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
    EMD_F       UMETA(DisplayName = "F"),
    EMD_B       UMETA(DisplayName = "B"),
    EMD_LL       UMETA(DisplayName = "LL"),
    EMD_LR       UMETA(DisplayName = "LR"),
    EMD_RL       UMETA(DisplayName = "RL"),
    EMD_RR       UMETA(DisplayName = "RR"),
};

UENUM(BlueprintType)
enum class EMovementDirectionBias : uint8
{
    EMDB_LeftFootForward       UMETA(DisplayName = "LeftFootForward"),
    EMDB_RightFootForward      UMETA(DisplayName = "RightFootForward"),
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECharacterStanceType StanceType = ECharacterStanceType::ECST_Stand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECharacterMoveMode MoveMode = ECharacterMoveMode::ECST_OnGround;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECharacterMovementType MovementType = ECharacterMovementType::ECMT_Walk;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveInput = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CameraTrace = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform CharacterTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool HasItemInHands = false;
};

namespace RealisticModes
{
    static const FName Walk = FName("RealisticWalk");
    static const FName Air = FName("RealisticAir");
}


USTRUCT(BlueprintType)
struct HANDITEMSYSTEM_API FInventoryViewNetworkSmoothingState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled = false;

    // Used to track the time of the last server move.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
    float ServerTime{ 0.0f };

    // Used to track client time as we try to match the server.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
    float ClientTime{ 0.0f };

    // Used for remembering how much time passed between server corrections.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
    float Duration{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator InitialRotation{ ForceInit };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator TargetRotation{ ForceInit };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator CurrentRotation{ ForceInit };
};

USTRUCT(BlueprintType)
struct HANDITEMSYSTEM_API FInventoryViewState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FInventoryViewNetworkSmoothingState NetworkSmoothing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCharacterAnimationData CharacterAnimationData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation{ ForceInit };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "deg/s"))
    float YawSpeed{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
    float PreviousYawAngle{ 0.0f };
};