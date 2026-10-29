#include "InOutInteract.h"
#include "GameFramework/Character.h"
#include "InventoryEcosystem/InventoryCore/Interface/InventoryMeshInterface.h"

void UInOutInteract::StartInteraction(APawn* TargetPlayer)
{
    if (!IsValid(TargetPlayer)) return;

    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(TargetPlayer);
    if (!LInterface) return;
    
    USkeletalMeshComponent* Mesh = LInterface->Execute_I_GetMesh(TargetPlayer);
    if (!IsValid(Mesh)) return;
    if (!Mesh->GetAnimInstance()) return;
    
    RunInteract(TPSBeginAnimMontage, TargetPlayer);
    OnStartAnimation.Broadcast(TargetPlayer, this);
    
    Mesh->GetAnimInstance()->Montage_Play(TPSBeginAnimMontage);
}

void UInOutInteract::EndInteraction(APawn* TargetPlayer)
{
    if (!IsValid(TargetPlayer)) return;

    auto LInterface = UInventoryCoreLibrary::HasMeshInterface(TargetPlayer);
    if (!LInterface) return;
    
    USkeletalMeshComponent* Mesh = LInterface->Execute_I_GetMesh(TargetPlayer);
    if (!IsValid(Mesh)) return;
    if (!Mesh->GetAnimInstance()) return;
	
    CancelInteract(TPSEndAnimMontage, TargetPlayer);
    OnEndAnimation.Broadcast(TargetPlayer, this);
    
    Mesh->GetAnimInstance()->Montage_Play(TPSEndAnimMontage);
}