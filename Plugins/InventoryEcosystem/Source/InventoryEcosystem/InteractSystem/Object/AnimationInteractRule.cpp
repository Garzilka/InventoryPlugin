#include "AnimationInteractRule.h"

#include "TimerManager.h"
#include "InventoryEcosystem/InteractSystem/AnimNotify/AnimNotify_Interacted.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"


void UAnimationInteractRule::InteractCallback(APawn* Target)
{
	if (!(IsPossiblyAllocatedUObjectPointer(Target) && IsValid(Target))) return;
	OnInteracted.Broadcast(Target, this);
}

void UAnimationInteractRule::RunInteract(UAnimMontage* Montage, APawn* TargetPlayer)
{
	if (!Montage)
	{
		InteractCallback(TargetPlayer);
		return;
	}
	
	FName NameNotify = FName(UAnimNotify_Interacted::StaticClass()->GetName());
	float LTime = Montage->GetPlayLength();
	for (auto notify : Montage->Notifies)
	{
		if (notify.NotifyName != NameNotify) continue;
		
		LTime = (1.f/Montage->RateScale) * notify.GetTriggerTime();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(Timer_TimeToInteract);
	
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, TEXT("InteractCallback"), TargetPlayer);			
	GetWorld()->GetTimerManager().SetTimer(Timer_TimeToInteract, Delegate, LTime, false);
}

void UAnimationInteractRule::CancelInteractCallback(APawn* Target)
{
	if (!(IsPossiblyAllocatedUObjectPointer(Target) && IsValid(Target))) return;
	OnCancelInteract.Broadcast(Target, this);
}

void UAnimationInteractRule::CancelInteract(UAnimMontage* Montage, APawn* TargetPlayer)
{
	if (!Montage)
	{
		CancelInteractCallback(TargetPlayer);
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(Timer_CancelInteract);
	
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, TEXT("CancelInteractCallback"), TargetPlayer);			
	GetWorld()->GetTimerManager().SetTimer(Timer_CancelInteract, Delegate, Montage->GetPlayLength(), false);
}
