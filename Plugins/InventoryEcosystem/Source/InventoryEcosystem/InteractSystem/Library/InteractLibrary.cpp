// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractLibrary.h"

#include "InventoryEcosystem/InventoryTools/Library/InventoryToolsLibrary.h"
#include "GameFramework/Character.h"
#include "InventoryEcosystem/InteractSystem/Components/BaseInteractComponent.h"
#include "InventoryEcosystem/InteractSystem/Components/InteractAnimationComponent.h"
#include "Kismet/KismetSystemLibrary.h"

bool UInteractLibrary::CheckDotAngle(FVector From, UBaseInteractComponent* LInteractComponent)
{
	if (!(LInteractComponent && IsValid(LInteractComponent->GetOwner()))) return false;
	if (!LInteractComponent->GetAngleSettings().bWithAngleCheck) return true;
	
	FRotator DotCheck = FRotator(LInteractComponent->GetOwner()->GetActorRotation().Pitch, LInteractComponent->GetOwner()->GetActorRotation().Yaw + LInteractComponent->GetAngleSettings().InteractAngleOffset, LInteractComponent->GetOwner()->GetActorRotation().Roll);
	
	return UInventoryToolsLibrary::CheckDotAngle(From, LInteractComponent->GetOwner()->GetActorLocation(), DotCheck, LInteractComponent->GetAngleSettings().InteractAngle);
}

bool UInteractLibrary::LineTrace(FInteractTraceData TraceData, TArray<FHitResult>& OutPut, FInteractDebugData IsShowDrawDebug)
{
	FHitResult Result;
	if (UKismetSystemLibrary::LineTraceSingleByProfile(
			TraceData.Pawn,
			TraceData.CameraLocation,
			TraceData.EndVector,
			TraceData.InteractProfile,
			false,
			{TraceData.Pawn},
			(IsShowDrawDebug.IsEnabled ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::Type::None),
			Result, 
			true))
	{
		OutPut.Add(Result);
	}
	return (OutPut.Num() > 0);
}



bool UInteractLibrary::PerformTrace(FInteractTraceData TraceData, TArray<FHitResult>& OutPut,	FInteractDebugData IsShowDrawDebug)
{
	bool Result = UKismetSystemLibrary::CapsuleTraceMultiByProfile(
		TraceData.Pawn->GetWorld(),
		TraceData.CameraLocation,
		TraceData.EndVector,
		TraceData.InteractionSphereSize,
		TraceData.InteractionSphereSize,
		TraceData.InteractProfile,
		false,
		{ TraceData.Pawn },
		(IsShowDrawDebug.IsEnabled ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::Type::None),
		OutPut,
		true);

	return Result;
}

UInteractAnimationComponent* UInteractLibrary::GetInteractAnimationComponent(UBaseInteractComponent* From)
{
	if (!(IsValid(From) && From->GetOwner())) return nullptr;
	if (From->GetInteractAnimationComponent()) return From->GetInteractAnimationComponent();
	TArray<UInteractAnimationComponent*> Components;
	From->GetOwner()->GetComponents<UInteractAnimationComponent>(Components);

	UInteractAnimationComponent* Result = nullptr;
	for (auto Component : Components)
	{
		if (!Result)
		{
			Result = Component;
			continue;
		}
		
		float Distance_1 = FVector::Distance(From->GetComponentLocation(), Result->GetComponentLocation());
		float Distance_2 = FVector::Distance(From->GetComponentLocation(), Component->GetComponentLocation());
		if (Distance_2 < Distance_1)
		{
			Result = Component;
		}
	}

	return Result;
}

UInteractAnimationComponent* UInteractLibrary::GetInteractAnimationFromActor(AActor* From)
{
	if (!IsValid(From)) return nullptr;
	TArray<UInteractAnimationComponent*> Components;
	From->GetOwner()->GetComponents<UInteractAnimationComponent>(Components);

	for (auto Component : Components)
	{
		return Component;
	}

	return nullptr;
}
