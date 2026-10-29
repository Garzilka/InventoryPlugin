// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryCameraAnimInstance.h"

#include "InventoryEcosystem/HandItemSystem/Character/BaseCharacter.h"
#include "InventoryEcosystem/HandItemSystem/Interface/GetCharacterData.h"
#include "InventoryEcosystem/HandItemSystem/Components/InventoryCamera/InventoryCameraComponent.h"

void UInventoryCameraAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Camera = Cast<UInventoryCameraComponent>(GetSkelMeshComponent());
    Character = Cast<ABaseCharacter>(GetOwningActor());

#if WITH_EDITOR
    if (!GetWorld()->IsGameWorld())
    {
        // Use default objects for editor preview.

        if (!IsValid(Camera))
        {
            Camera = GetMutableDefault<UInventoryCameraComponent>();
        }
    }
#endif

    if (Character == nullptr) return;
    if (UKismetSystemLibrary::DoesImplementInterface(Character, UGetCharacterData::StaticClass()))
    {
        Interface = Cast<IGetCharacterData>(Character);
    }
}

void UInventoryCameraAnimInstance::NativeUpdateAnimation(const float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if (!IsValid(Camera) || !IsValid(Character)) return;
    
    Interface = Cast<IGetCharacterData>(Character);
    if (!Interface) return;    
    
    auto Data = Interface->Execute_I_GetAnimationData(Character);
    ViewMode = ECharacterViewType::ECVT_FPS;
    MovementType = Data.MovementType;
    StanceType = Data.StanceType;
    bRightShoulder = true;
}
