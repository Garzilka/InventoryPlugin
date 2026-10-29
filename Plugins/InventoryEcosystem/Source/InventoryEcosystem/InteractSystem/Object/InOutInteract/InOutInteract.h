#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InteractSystem/Object/AnimationInteractRule.h"
#include "InOutInteract.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTSYSTEM_API UInOutInteract : public UAnimationInteractRule
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций", meta = (AllowPrivateAccess = "true", DisplayName = "Стартовая анимация персонажа"))
	UAnimMontage* TPSBeginAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций", meta = (AllowPrivateAccess = "true", DisplayName = "Анимация выхода"))
	UAnimMontage* TPSEndAnimMontage;

public:
	virtual void StartInteraction(APawn* TargetPlayer) override;
	virtual void EndInteraction(APawn* TargetPlayer) override;
	virtual UAnimMontage* GetPreviewAnimation() override {return TPSBeginAnimMontage;};
};