#include "SimpleAnimatedInteract.h"

#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"

void USimpleAnimatedInteract::StartInteraction(APawn* TargetPlayer)
{	
    if (!IsValid(TargetPlayer)) return;

    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(TargetPlayer);
    if (!LInterface) return;
    
    USkeletalMeshComponent* Mesh = LInterface->Execute_I_GetMesh(TargetPlayer);
    if (!IsValid(Mesh)) return;
    if (!Mesh->GetAnimInstance()) return;
    
    OnStartAnimation.Broadcast(TargetPlayer, this);
    RunInteract(TPSInteractAnimMontage, TargetPlayer);
    CancelInteract(TPSInteractAnimMontage, TargetPlayer);
    Mesh->GetAnimInstance()->Montage_Play(TPSInteractAnimMontage);
}
