#pragma once

#include "GameplayTagContainer.h"
#include "Runtime/AnimGraphRuntime/Public/AnimNodes/AnimNode_BlendListBase.h"
#include "GameplayTagsBlend.generated.h"

USTRUCT()
struct HANDITEMSYSTEM_API FAnimNode_GameplayTagsBlend : public FAnimNode_BlendListBase
{
    GENERATED_BODY()

public:
    FAnimNode_GameplayTagsBlend() = default;
    virtual ~FAnimNode_GameplayTagsBlend() = default;
#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty, PinShownByDefault))
    FGameplayTag ActiveTag;

    UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty))
    TArray<FGameplayTag> Tags;
#endif

protected:
    virtual int32 GetActiveChildIndex() override;

public:
    const FGameplayTag& GetActiveTag() const;

    const TArray<FGameplayTag>& GetTags() const;

#if WITH_EDITOR
    void RefreshPoses();
#endif
};
