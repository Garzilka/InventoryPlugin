// © 2025 mrbaconvn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h" 
#include "InventoryEcosystem/HandItemSystem/Objects/Action/ComboActionPass.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 5
#include "StructUtils/InstancedStruct.h"
#else 
#include "InstancedStruct.h"
#endif

#include "ComboActionPass_ActivateAbility.generated.h"

struct FAbilityContext;
class UGameplayAbility;

UCLASS(DisplayName = "GAS activate context ability", meta = (ToolTip = "Activate GAS ability with payload"))
class HANDITEMSYSTEM_API UComboActionPass_ActivateAbility : public UComboActionPass
{
	GENERATED_BODY()

public:
	virtual TArray<FSoftObjectPath> GetPreviewAssets() const override;
	virtual FString GetPassInformation_Implementation() const override;
	virtual auto ExecutePass_Implementation(UComboGraphInstance* GraphInstance) const -> bool override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TInstancedStruct<FAbilityContext> AbilityPayload;
};
