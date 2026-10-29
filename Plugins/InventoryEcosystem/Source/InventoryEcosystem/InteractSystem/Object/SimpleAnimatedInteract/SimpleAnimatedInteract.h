#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InteractSystem/Object/AnimationInteractRule.h"
#include "SimpleAnimatedInteract.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTSYSTEM_API USimpleAnimatedInteract : public UAnimationInteractRule
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций", meta = (AllowPrivateAccess = "true", DisplayName = "По высоте"))
	bool ByHeight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций", meta = (AllowPrivateAccess = "true", DisplayName = "Относительная анимация взаимодействия", 
		ToolTip = "Учитывайте размер персонажа и местоположение его PivotPoint\nПо умолчанию для станадртного персонажа:\n@-100 - пол\n@-20 - Стол \n@20 - шкаф", EditCondition = "ByHeight", EditConditionHides))
	TMap<int32, UAnimMontage*> TPSAnimationByHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Настройка анимаций", meta = (AllowPrivateAccess = "true", DisplayName = "Анимация взаимодействия", EditCondition = "!ByHeight", EditConditionHides))
	UAnimMontage* TPSInteractAnimMontage;

public:
	virtual void StartInteraction(APawn* TargetPlayer) override;
	virtual UAnimMontage* GetPreviewAnimation() override {return TPSInteractAnimMontage;};
};