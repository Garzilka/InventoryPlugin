// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "UObject/Object.h"
#include "ItemThumbnail.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMEDITOR_API UItemThumbnail : public UThumbnailRenderer
{
    GENERATED_BODY()
    
    // UThumbnailRenderer interface
    virtual bool CanVisualizeAsset(UObject* Object) override;
    virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas, bool bAdditionalViewFamily) override;
    virtual EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const override;
    // End of UThumbnailRenderer interface

};
