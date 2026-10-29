// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryToolsLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Animation/AnimNotifies/AnimNotify.h"

void UInventoryToolsLibrary::DrawDebugSphereAlternative(const UObject *WorldContext, const FVector &Location,
    const FRotator &Rotation, float Radius, const FLinearColor &Color, float Duration, float Thickness,
    uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
    const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
    if (IsValid(World))       return;
    

    const auto FColor{Color.ToFColor(true)};
    const auto bPersistent{Duration < 0.0f};

    const FRotationMatrix RotationMatrix{Rotation};

    FVector XAxis, YAxis, ZAxis;
    RotationMatrix.GetScaledAxes(XAxis, YAxis, ZAxis);

    DrawCircle(World, Location, XAxis, YAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
    DrawCircle(World, Location, XAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
    DrawCircle(World, Location, YAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

bool UInventoryToolsLibrary::ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName)
{
    return false;
}

void UInventoryToolsLibrary::DrawDebugSweepSphere(const UObject *WorldContext, const FVector &Start, const FVector &End,
    float Radius, const FLinearColor &Color, float Duration, float Thickness, uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
    const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
    if (IsValid(World)) return;
    

    const auto FColor{Color.ToFColor(true)};
    const auto bPersistent{Duration < 0.0f};

    const auto AxisVector{End - Start};

    DrawDebugCapsule(World, Start + AxisVector * 0.5f, UE_REAL_TO_FLOAT(AxisVector.Size()) * 0.5f + Radius,
                     Radius, FRotationMatrix::MakeFromZ(AxisVector).ToQuat(),
                     FColor, bPersistent, Duration, DepthPriority, Thickness);

    DrawDebugDirectionalArrow(World, Start, End, DrawArrowSize, FColor, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

float UInventoryToolsLibrary::GetTimeToNotify(UAnimMontage* FromMontage, TSubclassOf<UAnimNotify> Notify, bool WithSclae /*=true*/)
{
	if (!UKismetSystemLibrary::IsValidClass(Notify)) return -1.f;

	return UInventoryToolsLibrary::GetTimeToNotify(FromMontage, FName(Notify->GetName()));
}

float UInventoryToolsLibrary::GetTimeToNotify(UAnimMontage* FromMontage, FName NameNotify, bool WithSclae /*=true*/)
{
	TArray<FAnimNotifyEvent> Motifyes;

	for (auto LNotify : FromMontage->Notifies)
	{
		if (LNotify.NotifyName != NameNotify) continue;

		return (WithSclae ? ((1.f / FromMontage->RateScale) * LNotify.GetTriggerTime()) : LNotify.GetTriggerTime());
	}
	return -1.f;
}

bool UInventoryToolsLibrary::CheckDotAngle(FVector From, FVector To, FRotator DotCheck, float MaxAngle)
{
	FVector NormDirToPlayer = (From - To).GetSafeNormal();
	float LRadialAngle = FVector::DotProduct(NormDirToPlayer, DotCheck.Vector());
	float LAngel = UKismetMathLibrary::DegAcos(LRadialAngle);

	return (FMath::Abs(LAngel) < (MaxAngle / 2.f));
}

bool UInventoryToolsLibrary::IsInGameView()
{
#if WITH_EDITOR
    UEditorEngine* LocalEditor = GEditor;
    if (LocalEditor && LocalEditor->GetActiveViewport() && GEditor->GetActiveViewport()->GetClient())
    {
        return GEditor->GetActiveViewport()->GetClient()->IsInGameView();
    }
#endif

    return true;
}

FName UInventoryToolsLibrary::GetSimpleTagName(const FGameplayTag &Tag)
{
    const auto TagNode{UGameplayTagsManager::Get().FindTagNode(Tag)};

    return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
}