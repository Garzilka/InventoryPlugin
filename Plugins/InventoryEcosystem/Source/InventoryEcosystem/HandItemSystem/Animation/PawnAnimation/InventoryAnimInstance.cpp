// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryAnimInstance.h"

#include "InventoryEcosystem/HandItemSystem/Components/HandItemManager/BaseHandItemManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "InventoryEcosystem/HandItemSystem/Character/BaseCharacter.h"

USkeletalMeshComponent * UInventoryAnimInstance::GetMesh()
{
    if (IsValid(_PRIVATE_Mesh)) return _PRIVATE_Mesh;
    
    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(GetOwningActor());
    if (!LInterface) return nullptr;

    _PRIVATE_Mesh = LInterface->Execute_I_GetMesh(GetOwningActor());
    return _PRIVATE_Mesh;
}

bool UInventoryAnimInstance::HasWeapon()
{
    if (!IsValid(WeaponActor)) return false;
    return true;
}

bool UInventoryAnimInstance::IsLeftHandIK()
{
    if (!GetPlayerCharacter()) return false;

    if (HandItemManager && HandItemManager->InActiveAction())
    {
        return false;
    }

    if (HasWeapon())
    {
        return (!WeaponActor->IsReload());
    }
    return false;
}

void UInventoryAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ABaseCharacter>(GetOwningActor());
    if (!GetPlayerCharacter()) return;
    // #if WITH_EDITOR
    // 	if (!GetWorld()->IsGameWorld() && !IsValid(Character))
    // 	{
    // 		// Use default objects for editor preview.
    // 		Character = GetMutableDefault<ABaseCharacter>();
    // 	}
    // #endif
    if (!IsValid(GetPlayerCharacter())) return;
	
    HandItemManager = UBaseHandItemManager::GetBaseHandItemManager(GetPlayerCharacter());
    if (!IsValid(HandItemManager)) return;

    HandItemManager->OnNewCurrentSlot.AddUniqueDynamic(this, &UInventoryAnimInstance::NewWeapon);
}

void UInventoryAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
    Super::NativeUpdateAnimation(InDeltaSeconds);
	
    CurrentDeltaSeconds = InDeltaSeconds;
    if (!WeaponActor) return;
    if (!GetPlayerCharacter()) return;
	
    bIsFPS = false;
    // if (bIsFPS)
    // {
    // 	bIsFPS &= (GetPlayerCharacter()->GetEquipManager()->IsFPSMode());
    // }
    //Поднимаем правую руку до прицела
    const FTransform SightSocketTransform = WeaponActor->GetSightSocketTransform();
    const FTransform Hand_RTransform = GetMesh()->GetSocketTransform(RightHandBone);
	
    FinalRelativeHand = UKismetMathLibrary::MakeRelativeTransform(SightSocketTransform, Hand_RTransform);
    SightTransform = GetMesh()->GetSocketTransform((bIsFPS ? HeadBoneLocaly : HeadBone), RTS_ParentBoneSpace);

    //Дополнительное смещение оружия в камере
    AIMPoseOffsetLocation = WeaponActor->GetAIMPoseOffset(bIsFPS).GetLocation();
    AIMPoseOffsetRotation = FRotator(FQuat(WeaponActor->GetAIMPoseOffset(bIsFPS).Rotator()));
    BasePoseOffsetLocation = WeaponActor->GetBasePoseOffset(bIsFPS).GetLocation();
    BasePoseOffsetRotation = FRotator(FQuat(WeaponActor->GetBasePoseOffset(bIsFPS).Rotator()));

    WeaponSway(CurrentDeltaSeconds);

    if (bInterpRecoil)
    {
        RecoilInterpTo(CurrentDeltaSeconds);
    }

    SetLeftIK();
}

#pragma endregion //Engine


#pragma region UpdateWeaponAnimation

void UInventoryAnimInstance::SetLeftIK()
{
    if (!GetMesh()) return;
    if (!WeaponActor->IsUseIK())
    {
        return;
    }
    FTransform LeftIK = WeaponActor->GetLeftHandIK();
    JoinTargetLocation = WeaponActor->GetJoinTargetLocation();

    FVector OutPosition;
    FRotator OutRotation;
    GetMesh()->TransformToBoneSpace(RightHandBone, LeftIK.GetLocation(), LeftIK.Rotator(), OutPosition, OutRotation);
    LeftHandIKTransform = FTransform(OutRotation, OutPosition, FVector(1.f));
}

void UInventoryAnimInstance::WeaponSway(float DeltaSeconds)
{
    if (IsValid(WeaponActor))
    {
        if (const UCurveVector* Curve = WeaponActor->GetMovementSwayCurve())
        {
            const float OldVelocityMultiplier = VelocityMultiplier;
            // VelocityMultiplier = UKismetMathLibrary::NormalizeToRange(CharacterVelocity, 0.0f, 450/*Character sprint speed*/);
            // if (VelocityMultiplier < OldVelocityMultiplier)
            // {
            // 	VelocityMultiplier = UKismetMathLibrary::FInterpTo(OldVelocityMultiplier, VelocityMultiplier, DeltaSeconds, VelocityInterpSpeed);
            // }
            // if (VelocityMultiplier < 0.25f)
            // {
            // 	VelocityMultiplier = 0.25f;
            // }
            float Multiplier = 1.1f;
            if (WeaponActor->UseStatsForMovementSway())
            {
                const FFirearmStats Stats = WeaponActor->GetFirearmStats();
                Multiplier = Stats.Weight * (Stats.Weight * 3.0f / (Stats.Ergonomics * 1.5f)) + 1.0f;
            }

            SwayMultiplier = UKismetMathLibrary::FInterpTo(SwayMultiplier, WeaponActor->GetAimSwayMultiplier(), DeltaSeconds, InterpSpeed);
            SwayLocation *= SwayMultiplier;
            SwayRotation *= SwayMultiplier;
            // SwayMultiplier = UKismetMathLibrary::FInterpTo(SwayMultiplier, WeaponActor->GetAimSwayMultiplier(), DeltaSeconds, InterpSpeed);

            // CurveTimer += (DeltaSeconds * VelocityMultiplier);
            // FVector Graph = Curve->GetVectorValue(CurveTimer);
            // Graph *= VelocityMultiplier * Multiplier;
            // const FRotator Rotation = FRotator(Graph.Y, Graph.X, Graph.Z);
            // SwayLocation = Graph * SwayMultiplier;
            // SwayRotation = Rotation * SwayMultiplier;
            // SwayLocation *= SwayMultiplier;
            // SwayRotation *= SwayMultiplier;
        }
    }
}

void UInventoryAnimInstance::RecoilInterpToZero(float DeltaSeconds)
{
    FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaSeconds, 8.0f); // def = 6
    RecoilLocation = FinalRecoilTransform.GetLocation();
    RecoilRotation = FinalRecoilTransform.Rotator();
    if (RecoilLocation.Equals(FVector::ZeroVector, 0.1f) && RecoilRotation.Equals(FRotator::ZeroRotator, 0.1f))
    {
        bInterpRecoil = false;
    }
}

void UInventoryAnimInstance::RecoilInterpTo(float DeltaSeconds)
{
    if (WeaponActor)
    {
        const float CurrentTime = GetWorld()->GetTimeSeconds() - RecoilStartTime;
        const FRecoilData RecoilData = WeaponActor->GetRecoilData();

        if (RecoilData.RecoilAnimationData.RecoilLocationCurve)
        {
            const float Randomess = FMath::RandRange(RecoilData.RecoilAnimationData.RecoilLocationRandomness.X, RecoilData.RecoilAnimationData.RecoilLocationRandomness.Y);
            RecoilLocation += (RecoilData.RecoilAnimationData.RecoilLocationCurve->GetVectorValue(CurrentTime) * Randomess * WeaponActor->GetCurrentSettingControll().HorizontalRecoilCoefAnim);
            RecoilLocation *= WeaponActor->GetFirearmStats().RecoilMultiplier;
            FinalRecoilTransform.SetLocation(RecoilLocation);
        }
        if (RecoilData.RecoilAnimationData.RecoilRotationCurve)
        {
            const float RecoilCoef = WeaponActor->GetCurrentSettingControll().VericalRecoilCoefAnim;

            const FVector CurveData = RecoilData.RecoilAnimationData.RecoilRotationCurve->GetVectorValue(CurrentTime);
            const float PitchRandom = (CurveData.X * FMath::RandRange(RecoilData.RecoilAnimationData.RecoilPitchRandomness.X, RecoilData.RecoilAnimationData.RecoilPitchRandomness.Y)) * RecoilMultiplier * RecoilCoef;
            const float YawRandom = (CurveData.Y * FMath::RandRange(RecoilData.RecoilAnimationData.RecoilYawRandomness.X, RecoilData.RecoilAnimationData.RecoilYawRandomness.Y)) * RecoilMultiplier * RecoilCoef;
            const float RollRandom = (CurveData.Z * FMath::RandRange(RecoilData.RecoilAnimationData.RecoilRollRandomness.X, RecoilData.RecoilAnimationData.RecoilRollRandomness.Y)) * RecoilMultiplier * RecoilCoef;

            RecoilRotation += FRotator(RollRandom, PitchRandom, YawRandom);
            RecoilRotation *= WeaponActor->GetFirearmStats().RecoilMultiplier;
            FinalRecoilTransform.SetRotation(RecoilRotation.Quaternion());
        }
    }

    RecoilInterpToZero(DeltaSeconds);
}


#pragma endregion //UpdateWeaponAnimation


#pragma region TOOLS

ABaseCharacter* UInventoryAnimInstance::GetPlayerCharacter()
{
	if (!IsValid(Character))
	{
		Character = Cast<ABaseCharacter>(GetOwningActor());
	}
	return Character;
}

void UInventoryAnimInstance::NewWeapon(UBaseHandItemManager* HIM, AActor* NewActor, int32 Slot)
{
	WeaponActor = Cast<ABaseFireWeapon>(NewActor);
	if (!IsValid(WeaponActor)) return;
	CurrentSightSocketTransform = WeaponActor->GetSightSocketTransform();

	WeaponActor->OnWeaponFire.AddUniqueDynamic(this, &UInventoryAnimInstance::WeaponFire);
}

void UInventoryAnimInstance::WeaponFire(ABaseFireWeapon* Weapon)
{
	bInterpRecoil = true;
	RecoilStartTime = GetWorld()->GetTimeSeconds();
	RecoilMultiplier = 1.f;
}

void UInventoryAnimInstance::WeaponFireStatus(ABaseFireWeapon* Weapon, bool IsFire)
{
	RecoilMultiplier = 0.f;
	if (!IsFire) return;
	bInterpRecoil = true;
	RecoilStartTime = GetWorld()->GetTimeSeconds();
	RecoilMultiplier = 1.f;
}

#pragma endregion TOOLS
