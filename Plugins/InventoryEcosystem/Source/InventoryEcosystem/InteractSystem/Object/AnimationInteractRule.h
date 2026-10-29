#pragma once

#include "InventoryEcosystem/InventoryCore/Object/CoreInventoryObject.h"
#include "AnimationInteractRule.generated.h"


class UInteractAnimationComponent;

/**
* Вызывается когда объект попал в поле взаимодействия или вышел из него (Локально + Сервер)
* @Character - Кто взаимодйствует
* @InteractComponent - Референс на себя
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAnimationInteractRuleSignature, class APawn*, CharacterInstigator, class UAnimationInteractRule*, InteractRules);

UCLASS(EditInlineNew, DefaultToInstanced, Abstract)
class INTERACTSYSTEM_API UAnimationInteractRule : public UCoreInventoryObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FAnimationInteractRuleSignature OnInteracted;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FAnimationInteractRuleSignature OnCancelInteract;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FAnimationInteractRuleSignature OnStartAnimation;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FAnimationInteractRuleSignature OnEndAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций")
	float PreviewTime = 0.f;

protected:
	
	UPROPERTY()
	FTimerHandle Timer_TimeToInteract;
	
	UPROPERTY()
	FTimerHandle Timer_CancelInteract;
public:

	UFUNCTION()
	virtual void StartInteraction(APawn* TargetPlayer) { return; }

	UFUNCTION()
	virtual void EndInteraction(APawn* TargetPlayer) { return; }

	virtual UAnimMontage* GetPreviewAnimation() {return nullptr;};
protected:
	UFUNCTION()
	void InteractCallback(APawn* Target);

	void RunInteract(UAnimMontage* Montage, APawn* TargetPlayer);
	
	UFUNCTION()
	void CancelInteractCallback(APawn* Target);

	void CancelInteract(UAnimMontage* Montage, APawn* TargetPlayer);
	
};
