#pragma once


#include "InventoryEcosystem/HandItemSystem/Data/Node/GameplayTagsBlend.h"
#include "Editor/AnimGraph/Public/AnimGraphNode_BlendListBase.h"
#include "AnimGraphNode_GameplayTagBlend.generated.h"

UCLASS()
class HANDITEMSYSTEMEDITOR_API UAnimGraphNode_GameplayTagBlend : public UAnimGraphNode_BlendListBase
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    FAnimNode_GameplayTagsBlend Node;

public:
    UAnimGraphNode_GameplayTagBlend();

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

    virtual FText GetTooltipText() const override;

    virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins) override;

    virtual FString GetNodeCategory() const override;

    virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 PinIndex) const override;

protected:
    static void GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin);
};