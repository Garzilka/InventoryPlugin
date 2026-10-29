
#include "InventoryCameraComponent.h"

#include "MoverComponent.h"
#include "Components/CapsuleComponent.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryMathLib.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "InventoryEcosystem/HandItemSystem/Library/CameraConstants.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "InventoryEcosystem/InventoryCore/Library/InventoryCoreLibrary.h"


UInventoryCameraComponent::UInventoryCameraComponent()
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
    bTickInEditor = false;
    bHiddenInGame = true;
}

void UInventoryCameraComponent::OnRegister()
{
    Super::OnRegister();
}

void UInventoryCameraComponent::RegisterComponentTickFunctions(const bool bRegister)
{
    Super::RegisterComponentTickFunctions(bRegister);
    AddTickPrerequisiteActor(GetOwner());
}

void UInventoryCameraComponent::Activate(const bool bReset)
{
    if (!bReset && !ShouldActivate())
    {
    	Super::Activate(bReset);
    	return;
    }
    Super::Activate(bReset);
    TickCamera(0.0f, false);
}

void UInventoryCameraComponent::InitAnim(const bool bForceReinitialize)
{
    Super::InitAnim(bForceReinitialize);
    AnimationInstance = GetAnimInstance();
}

void UInventoryCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryCameraComponent::TickComponent(float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (IsValid(Settings) && Settings->bIgnoreTimeDilation)
    {
    	// Используется предыдущее глобальное замедление времени, так как это может еще не повлиять на дельта-время этого кадра,
    	// таким образом, при отмене масштабирования будет получено неправильное значение дельта-времени.
    	const auto TimeDilation{PreviousGlobalTimeDilation * GetOwner()->CustomTimeDilation};
    	DeltaTime = TimeDilation > UE_SMALL_NUMBER ? DeltaTime / TimeDilation : GetWorld()->DeltaRealTimeSeconds;
    }
    
    PreviousGlobalTimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Skip camera tick until parallel animation evaluation completes.
    if (!IsRunningParallelEvaluation())
    {
    	TickCamera(DeltaTime);
    }
}

void UInventoryCameraComponent::CompleteParallelAnimationEvaluation(const bool bDoPostAnimationEvaluation)
{
	Super::CompleteParallelAnimationEvaluation(bDoPostAnimationEvaluation);

	TickCamera(GetAnimInstance()->GetDeltaSeconds());
}



FVector UInventoryCameraComponent::GetFirstPersonCameraLocation()
{
    return GetPawnMesh()->GetSocketLocation(Settings->FirstPerson.CameraSocketName);
}

FVector UInventoryCameraComponent::GetThirdPersonPivotLocation()
{
    const auto* Mesh{GetPawnMesh()};
    if (!Mesh) return FVector::ZeroVector;
    
    FVector FirstPivotLocation;
    if (!IsValid(Mesh->GetAttachParent()) && Settings->ThirdPerson.FirstPivotSocketName == UCameraConstants::RootBoneName())
    {
    	// Расположение корневой кости обычно остается неизменным, когда меш отсоединяется, поэтому используется расположение капсулы в качестве запасного варианта.
    	FirstPivotLocation = GetOwningPlayerPawn()->GetRootComponent()->GetComponentLocation();
    	FirstPivotLocation.Z -= GetOwningPlayerPawn()->GetRootComponent()->Bounds.BoxExtent.Z;
    }
    else
    {
    	FirstPivotLocation = Mesh->GetSocketLocation(Settings->ThirdPerson.FirstPivotSocketName);
    }
    return (FirstPivotLocation + Mesh->GetSocketLocation(Settings->ThirdPerson.SecondPivotSocketName)) * 0.5f;
}

FVector UInventoryCameraComponent::GetThirdPersonTraceStartLocation()
{
	return GetPawnMesh()->GetSocketLocation(bRightShoulder
		                                               ? Settings->ThirdPerson.TraceShoulderRightSocketName
		                                               : Settings->ThirdPerson.TraceShoulderLeftSocketName);
}

void UInventoryCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
    ViewInfo.Location = CameraLocation;
    ViewInfo.Rotation = CameraRotation;
    ViewInfo.FOV = CameraFieldOfView;
    ViewInfo.PostProcessBlendWeight = IsValid(Settings) ? PostProcessWeight : 0.0f;
    if (ViewInfo.PostProcessBlendWeight > UE_SMALL_NUMBER)
    {
    	ViewInfo.PostProcessSettings = Settings->PostProcess;
    }
}

void UInventoryCameraComponent::TickCamera(const float DeltaTime, bool bAllowLag)
{
    if (!IsValid(GetAnimInstance()) || !IsValid(Settings) || !IsValid(GetOwningPlayerPawn()))
    {
        return;
    }

    const auto bDisplayDebugCameraShapes = true;


    const UMoverComponent* MoverComp = GetOwningPlayerPawn()->GetComponentByClass<UMoverComponent>();
    if (!MoverComp) return;
    // Refresh movement base.

    const auto bMovementBaseHasRelativeRotation{true};

    auto MovementBaseLocation{FVector::ZeroVector};
    auto MovementBaseRotation{FQuat::Identity};

    if (bMovementBaseHasRelativeRotation)
    {
    	MovementBaseUtility::GetMovementBaseTransform(MoverComp->GetMovementBase(), MoverComp->GetMovementBaseBoneName(),
	                                              MovementBaseLocation, MovementBaseRotation);
    }

    if (MoverComp->GetMovementBase() != MovementBasePrimitive || MoverComp->GetMovementBaseBoneName() != MovementBaseBoneName)
    {
        MovementBasePrimitive = MoverComp->GetMovementBase();
        MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

	if (bMovementBaseHasRelativeRotation)
	{
		const auto MovementBaseRotationInverse{MovementBaseRotation.Inverse()};
		PivotMovementBaseRelativeLagLocation = MovementBaseRotationInverse.RotateVector(PivotLagLocation - MovementBaseLocation);
		CameraMovementBaseRelativeRotation = MovementBaseRotationInverse * CameraRotation.Quaternion();
	}
	else
	{
		PivotMovementBaseRelativeLagLocation = FVector::ZeroVector;
		CameraMovementBaseRelativeRotation = FQuat::Identity;
	}
    }

    const auto CameraTargetRotation{GetOwningPlayerPawn()->GetViewRotation()};

    const auto PreviousPivotTargetLocation{PivotTargetLocation};

    PivotTargetLocation = GetThirdPersonPivotLocation();

    const auto FirstPersonOverride{
		UInventoryMathLib::Clamp01(GetAnimInstance()->GetCurveValue(UCameraConstants::FirstPersonOverrideCurveName()))
	};

    if (FAnimWeight::IsFullWeight(FirstPersonOverride))
    {
        PivotLagLocation = PivotTargetLocation;
	PivotLocation = PivotTargetLocation;

	CameraLocation = GetFirstPersonCameraLocation();
	CameraRotation = CameraTargetRotation;

	CameraFieldOfView = bOverrideFieldOfView ? FieldOfViewOverride : Settings->FirstPerson.FieldOfView;
	return;
    }

    // Принудительное отключение задержки камеры, если персонаж был телепортирован.

    bAllowLag &= Settings->TeleportDistanceThreshold <= 0.0f ||
	FVector::DistSquared(PreviousPivotTargetLocation, PivotTargetLocation) <= FMath::Square(Settings->TeleportDistanceThreshold);

    // Расчет поворота камеры.

    if (bMovementBaseHasRelativeRotation)
    {
    	CameraRotation = (MovementBaseRotation * CameraMovementBaseRelativeRotation).Rotator();
	CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);
	CameraMovementBaseRelativeRotation = MovementBaseRotation.Inverse() * CameraRotation.Quaternion();
    }
    else
    {
        CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);
    }

    const FRotator CameraYawRotation{0.0f, CameraRotation.Yaw, 0.0f};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UInventoryToolsLibrary::DrawDebugSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation, 16.0f, FLinearColor::Green);
	}
#endif

    // Calculate pivot lag location. Get the pivot target location and interpolate using axis-independent lag for maximum control.
    if (bMovementBaseHasRelativeRotation)
    {
    	PivotLagLocation = MovementBaseLocation + MovementBaseRotation.RotateVector(PivotMovementBaseRelativeLagLocation);
    	PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation.Quaternion(), DeltaTime, bAllowLag);
    	PivotMovementBaseRelativeLagLocation = MovementBaseRotation.UnrotateVector(PivotLagLocation - MovementBaseLocation);
    }
    else
    {
    	PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation.Quaternion(), DeltaTime, bAllowLag);
    }

#if ENABLE_DRAW_DEBUG
    if (bDisplayDebugCameraShapes)
    {
    	DrawDebugLine(GetWorld(), PivotLagLocation, PivotTargetLocation,
    	              FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true),
    	              false, 0.0f, 0, UInventoryToolsLibrary::DrawLineThickness);
    	UInventoryToolsLibrary::DrawDebugSphereAlternative(GetWorld(), PivotLagLocation, CameraYawRotation, 16.0f, {1.0f, 0.5f, 0.0f});
    }
#endif
    
    // Calculate pivot location.
    const auto PivotOffset{CalculatePivotOffset()};
    PivotLocation = PivotLagLocation + PivotOffset;
    
#if ENABLE_DRAW_DEBUG
    if (bDisplayDebugCameraShapes)
    {
    	DrawDebugLine(GetWorld(), PivotLocation, PivotLagLocation,
    	              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
    	              false, 0.0f, 0, UInventoryToolsLibrary::DrawLineThickness);
    	UInventoryToolsLibrary::DrawDebugSphereAlternative(GetWorld(), PivotLocation, CameraYawRotation, 16.0f, {0.0f, 0.75f, 1.0f});
    }
#endif

    // Calculate target camera location.
    const auto CameraTargetLocation{PivotLocation + CalculateCameraOffset()};
    // Trace for an object between the camera and character to apply a corrective offset.
    const auto CameraResultLocation{CalculateCameraTrace(CameraTargetLocation, PivotOffset, DeltaTime, bAllowLag, TraceDistanceRatio)};
    if (!FAnimWeight::IsRelevant(FirstPersonOverride))
    {
    	CameraLocation = CameraResultLocation;
    	CameraFieldOfView = Settings->ThirdPerson.FieldOfView;
    }
    else
    {
    	CameraLocation = FMath::Lerp(CameraResultLocation, GetFirstPersonCameraLocation(), FirstPersonOverride);
    	CameraFieldOfView = FMath::Lerp(Settings->ThirdPerson.FieldOfView, Settings->FirstPerson.FieldOfView, FirstPersonOverride);
    }
    if (bOverrideFieldOfView)
    {
    	CameraFieldOfView = FieldOfViewOverride;
    }
    CameraFieldOfView = FMath::Clamp(CameraFieldOfView + CalculateFovOffset(), 5.0f, 175.0f);
}

FRotator UInventoryCameraComponent::CalculateCameraRotation(const FRotator& CameraTargetRotation, const float DeltaTime, const bool bAllowLag) const
{
    if (!bAllowLag)
    {
        return CameraTargetRotation;
    }
	
    const auto RotationLag{GetAnimInstance()->GetCurveValue(UCameraConstants::RotationLagCurveName())};
	
    if (!Settings->bEnableCameraLagSubstepping ||
        DeltaTime <= Settings->CameraLagSubstepping.LagSubstepDeltaTime ||
        RotationLag <= 0.0f)
    {
        return UInventoryMathLib::ExponentialDecay(CameraRotation, CameraTargetRotation, DeltaTime, RotationLag);
    }
	
    const auto CameraInitialRotation{CameraRotation};
    const auto SubstepRotationSpeed{(CameraTargetRotation - CameraInitialRotation).GetNormalized() * (1.0f / DeltaTime)};
	
    auto NewCameraRotation{CameraRotation};
    auto PreviousSubstepTime{0.0f};
	
    for (auto SubstepNumber{1};; SubstepNumber++)
    {
        const auto SubstepTime{SubstepNumber * Settings->CameraLagSubstepping.LagSubstepDeltaTime};
        if (SubstepTime < DeltaTime - UE_SMALL_NUMBER)
        {
            NewCameraRotation = FMath::RInterpTo(NewCameraRotation, CameraInitialRotation + SubstepRotationSpeed * SubstepTime,
                                                 SubstepTime - PreviousSubstepTime, RotationLag);
	
            PreviousSubstepTime = SubstepTime;
        }
        else
        {
            return FMath::RInterpTo(NewCameraRotation, CameraTargetRotation, DeltaTime - PreviousSubstepTime, RotationLag);
        }
    }
}

FVector UInventoryCameraComponent::CalculatePivotLagLocation(const FQuat& CameraYawRotation, const float DeltaTime, const bool bAllowLag) const
{
    if (!bAllowLag)
    {
    	return PivotTargetLocation;
    }
    
    const auto RelativePivotInitialLagLocation{CameraYawRotation.UnrotateVector(PivotLagLocation)};
    const auto RelativePivotTargetLocation{CameraYawRotation.UnrotateVector(PivotTargetLocation)};
    
    const auto LocationLagX{GetAnimInstance()->GetCurveValue(UCameraConstants::LocationLagXCurveName())};
    const auto LocationLagY{GetAnimInstance()->GetCurveValue(UCameraConstants::LocationLagYCurveName())};
    const auto LocationLagZ{GetAnimInstance()->GetCurveValue(UCameraConstants::LocationLagZCurveName())};
    
    if (!Settings->bEnableCameraLagSubstepping ||
        DeltaTime <= Settings->CameraLagSubstepping.LagSubstepDeltaTime ||
        (LocationLagX <= 0.0f && LocationLagY <= 0.0f && LocationLagZ <= 0.0f))
    {
    	return CameraYawRotation.RotateVector({
    		UInventoryMathLib::ExponentialDecay(RelativePivotInitialLagLocation.X, RelativePivotTargetLocation.X, DeltaTime, LocationLagX),
    		UInventoryMathLib::ExponentialDecay(RelativePivotInitialLagLocation.Y, RelativePivotTargetLocation.Y, DeltaTime, LocationLagY),
    		UInventoryMathLib::ExponentialDecay(RelativePivotInitialLagLocation.Z, RelativePivotTargetLocation.Z, DeltaTime, LocationLagZ)
    	});
    }
    
    const auto SubstepMovementSpeed{(RelativePivotTargetLocation - RelativePivotInitialLagLocation) / DeltaTime};
    
    auto RelativePivotLagLocation{RelativePivotInitialLagLocation};
    auto PreviousSubstepTime{0.0f};
	
    for (auto SubstepNumber{1};; SubstepNumber++)
    {
    	const auto SubstepTime{SubstepNumber * Settings->CameraLagSubstepping.LagSubstepDeltaTime};
    	if (SubstepTime < DeltaTime - UE_SMALL_NUMBER)
    	{
    		const auto SubstepRelativePivotTargetLocation{RelativePivotInitialLagLocation + SubstepMovementSpeed * SubstepTime};
    		const auto SubstepDeltaTime{SubstepTime - PreviousSubstepTime};
    
    		RelativePivotLagLocation.X = FMath::FInterpTo(RelativePivotLagLocation.X, SubstepRelativePivotTargetLocation.X,
    		                                              SubstepDeltaTime, LocationLagX);
    		RelativePivotLagLocation.Y = FMath::FInterpTo(RelativePivotLagLocation.Y, SubstepRelativePivotTargetLocation.Y,
    		                                              SubstepDeltaTime, LocationLagY);
    		RelativePivotLagLocation.Z = FMath::FInterpTo(RelativePivotLagLocation.Z, SubstepRelativePivotTargetLocation.Z,
    		                                              SubstepDeltaTime, LocationLagZ);
    
    		PreviousSubstepTime = SubstepTime;
    	}
    	else
    	{
    		const auto RemainingDeltaTime{DeltaTime - PreviousSubstepTime};
    
    		RelativePivotLagLocation.X = FMath::FInterpTo(RelativePivotLagLocation.X, RelativePivotTargetLocation.X,
    		                                              RemainingDeltaTime, LocationLagX);
    		RelativePivotLagLocation.Y = FMath::FInterpTo(RelativePivotLagLocation.Y, RelativePivotTargetLocation.Y,
    		                                              RemainingDeltaTime, LocationLagY);
    		RelativePivotLagLocation.Z = FMath::FInterpTo(RelativePivotLagLocation.Z, RelativePivotTargetLocation.Z,
    		                                              RemainingDeltaTime, LocationLagZ);
    
    		return CameraYawRotation.RotateVector(RelativePivotLagLocation);
    	}
    }
}

FVector UInventoryCameraComponent::CalculatePivotOffset()
{
    return GetPawnMesh()->GetComponentQuat().RotateVector(
    	FVector{
    		GetAnimInstance()->GetCurveValue(UCameraConstants::PivotOffsetXCurveName()),
    		GetAnimInstance()->GetCurveValue(UCameraConstants::PivotOffsetYCurveName()),
    		GetAnimInstance()->GetCurveValue(UCameraConstants::PivotOffsetZCurveName())
    	} * GetPawnMesh()->GetComponentScale().Z);
}

FVector UInventoryCameraComponent::CalculateCameraOffset()
{
    return CameraRotation.RotateVector(
    	FVector{
    		GetAnimInstance()->GetCurveValue(UCameraConstants::CameraOffsetXCurveName()),
    		GetAnimInstance()->GetCurveValue(UCameraConstants::CameraOffsetYCurveName()),
    		GetAnimInstance()->GetCurveValue(UCameraConstants::CameraOffsetZCurveName())
    	} * GetPawnMesh()->GetComponentScale().Z);
}

float UInventoryCameraComponent::CalculateFovOffset() const
{
	return GetAnimInstance()->GetCurveValue(UCameraConstants::FovOffsetCurveName());
}

FVector UInventoryCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio)
{
	
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{
		UInventoryToolsLibrary::ShouldDisplayDebugForActor(GetOwner(), UCameraConstants::CameraTracesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	const auto MeshScale{GetPawnMesh()->GetComponentScale().Z};

	static const FName MainTraceTag{FString::Printf(TEXT("%hs (Main Trace)"), __FUNCTION__)};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + FVector{Settings->ThirdPerson.TraceOverrideOffset},
			UInventoryMathLib::Clamp01(GetAnimInstance()->GetCurveValue(UCameraConstants::TraceOverrideCurveName())))
	};

	const auto TraceEnd{CameraTargetLocation};
	const auto CollisionShape{FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale)};

	auto TraceResult{TraceEnd};

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                     CollisionShape, {MainTraceTag, false, GetOwner()}))
	{
		if (!Hit.bStartPenetrating)
		{
			TraceResult = Hit.Location;
		}
		else if (TryAdjustLocationBlockedByGeometry(TraceStart, bDisplayDebugCameraTraces))
		{
			static const FName AdjustedTraceTag{FString::Printf(TEXT("%hs (Adjusted Trace)"), __FUNCTION__)};

			GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
			                                 CollisionShape, {AdjustedTraceTag, false, GetOwner()});
			if (Hit.IsValidBlockingHit())
			{
				TraceResult = Hit.Location;
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		UInventoryToolsLibrary::DrawDebugSweepSphere(GetWorld(), TraceStart, TraceResult, Settings->ThirdPerson.TraceRadius * MeshScale,
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	// Apply trace distance smoothing.

	if (!bAllowLag || !Settings->ThirdPerson.bEnableTraceDistanceSmoothing)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TraceVector{TraceEnd - TraceStart};
	const auto TraceDistance{TraceVector.Size()};

	if (TraceDistance <= UE_KINDA_SMALL_NUMBER)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TargetTraceDistanceRatio{UE_REAL_TO_FLOAT((TraceResult - TraceStart).Size() / TraceDistance)};

	// NewTraceDistanceRatio = TargetTraceDistanceRatio <= TraceDistanceRatio
	// 	                        ? TargetTraceDistanceRatio
	// 	                        : UInventoryMathLib::ExponentialDecay(TraceDistanceRatio, TargetTraceDistanceRatio, DeltaTime,
	// 	                                                     Settings->ThirdPerson.TraceDistanceSmoothing.InterpolationSpeed);

	return TraceStart + TraceVector * TraceDistanceRatio;
}

bool UInventoryCameraComponent::TryAdjustLocationBlockedByGeometry(FVector& Location, const bool bDisplayDebugCameraTraces)
{
	// Based on ComponentEncroachesBlockingGeometry_WithAdjustment().

	const auto MeshScale{GetPawnMesh()->GetComponentScale().Z};
	const auto CollisionShape{FCollisionShape::MakeSphere((Settings->ThirdPerson.TraceRadius + 1.0f) * MeshScale)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.IsEmpty())

	ON_SCOPE_EXIT
	{
		Overlaps.Reset();
	};

	static const FName OverlapMultiTraceTag{FString::Printf(TEXT("%hs (Overlap Multi)"), __FUNCTION__)};

	if (!GetWorld()->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                       CollisionShape, {OverlapMultiTraceTag, false, GetOwner()}))
	{
		return false;
	}

	auto Adjustment{FVector::ZeroVector};
	auto bAnyValidBlock{false};

	FMTDResult MtdResult;

	for (const auto& Overlap : Overlaps)
	{
		if (!Overlap.Component.IsValid() ||
		    Overlap.Component->GetCollisionResponseToChannel(Settings->ThirdPerson.TraceChannel) != ECR_Block)
		{
			continue;
		}

		const auto* OverlapBody{Overlap.Component->GetBodyInstance(NAME_None, true, Overlap.ItemIndex)};

		if (OverlapBody == nullptr || !OverlapBody->OverlapTest(Location, FQuat::Identity, CollisionShape, &MtdResult))
		{
			return false;
		}

		if (!FMath::IsNearlyZero(MtdResult.Distance))
		{
			Adjustment += MtdResult.Direction * MtdResult.Distance;
			bAnyValidBlock = true;
		}
	}

	if (!bAnyValidBlock)
	{
		return false;
	}

	auto AdjustmentDirection{Adjustment};

	if (!AdjustmentDirection.Normalize() ||
	    ((GetOwner()->GetActorLocation() - Location).GetSafeNormal() | AdjustmentDirection) < -UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}
//
// #if ENABLE_DRAW_DEBUG
// 	if (bDisplayDebugCameraTraces)
// 	{
// 		DrawDebugLine(GetWorld(), Location, Location + Adjustment,
// 		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
// 		              false, 5.0f, 0, UAlsUtility::DrawLineThickness);
// 	}
// #endif

	Location += Adjustment;

	static const FName FreeSpaceTraceTag{FString::Printf(TEXT("%hs (Free Space Overlap)"), __FUNCTION__)};

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                                 FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}
