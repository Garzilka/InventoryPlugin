// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryEcosystem/InventoryTools/Library/InventoryMathLib.h"
#include "InventoryEcosystem/InventoryCore/Component/InventoryCoreSkeletalMeshComponent.h"
#include "InventoryEcosystem/HandItemSystem/Data/CameraSettings.h"
#include "InventoryCameraComponent.generated.h"



UCLASS()
class HANDITEMSYSTEM_API UInventoryCameraComponent : public UInventoryCoreSkeletalMeshComponent
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
    TObjectPtr<UCameraSettings> Settings;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (InlineEditConditionToggle))
    bool bOverrideFieldOfView = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings",
    	Meta = (ClampMin = 5, ClampMax = 175, EditCondition = "bOverrideFieldOfView", ForceUnits = "deg"))
    float FieldOfViewOverride{90.0f};
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 1))
    float PostProcessWeight;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ShowInnerProperties))
    TWeakObjectPtr<UAnimInstance> AnimationInstance;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ForceUnits = "x"))
    float PreviousGlobalTimeDilation{1.0f};
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FVector PivotTargetLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FVector PivotLagLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FVector PivotLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FVector CameraLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FRotator CameraRotation;
    
    UPROPERTY(BlueprintReadOnly, Category = "State", Transient)
    TObjectPtr<UPrimitiveComponent> MovementBasePrimitive;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FName MovementBaseBoneName;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FVector PivotMovementBaseRelativeLagLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    FQuat CameraMovementBaseRelativeRotation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0, ClampMax = 1, ForceUnits = "%"))
    float TraceDistanceRatio{1.0f};
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 5, ClampMax = 175, ForceUnits = "deg"))
    float CameraFieldOfView{90.0f};
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
    bool bRightShoulder = true;
public:
    UInventoryCameraComponent();
    virtual void OnRegister() override;
    virtual void RegisterComponentTickFunctions(bool bRegister) override;
    virtual void Activate(bool bReset) override;
    virtual void InitAnim(bool bForceReinitialize) override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void CompleteParallelAnimationEvaluation(bool bDoPostAnimationEvaluation) override;
    
public:
    bool IsFieldOfViewOverriden() const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Camera")
    void SetFieldOfViewOverriden(bool bNewOverriden);
        
    float GetFieldOfViewOverride() const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Camera")
    void SetFieldOfViewOverride(float NewFieldOfView);
    
    float GetPostProcessWeight() const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Camera")
    void SetPostProcessWeight(float NewPostProcessWeight);
    
    bool IsRightShoulder() const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory|Camera")
    void SetRightShoulder(bool bNewRightShoulder);
    
    UFUNCTION(BlueprintPure, Category = "Inventory|Camera", Meta = (ReturnDisplayName = "Camera Location"))
    FVector GetFirstPersonCameraLocation();
    
    UFUNCTION(BlueprintPure, Category = "Inventory|Camera", Meta = (ReturnDisplayName = "Pivot Location"))
    FVector GetThirdPersonPivotLocation();
    
    UFUNCTION(BlueprintPure, Category = "Inventory|Camera", Meta = (ReturnDisplayName = "Trace Start"))
    FVector GetThirdPersonTraceStartLocation();

    UFUNCTION(BlueprintPure, Category = "Inventory|Camera")
    void GetViewInfo(FMinimalViewInfo& ViewInfo) const;

private:
    void TickCamera(float DeltaTime, bool bAllowLag = true);

    FRotator CalculateCameraRotation(const FRotator& CameraTargetRotation, float DeltaTime, bool bAllowLag) const;

    FVector CalculatePivotLagLocation(const FQuat& CameraYawRotation, float DeltaTime, bool bAllowLag) const;

    FVector CalculatePivotOffset();
    FVector CalculateCameraOffset();

    float CalculateFovOffset() const;

    FVector CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
	                             float DeltaTime, bool bAllowLag, float& NewTraceDistanceRatio);

    bool TryAdjustLocationBlockedByGeometry(FVector& Location, bool bDisplayDebugCameraTraces);
public:
    void DisplayDebug(const UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& VerticalLocation) const;

private:
    static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalLocation, float& VerticalLocation);

    void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

    void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

    void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;
};

inline bool UInventoryCameraComponent::IsFieldOfViewOverriden() const
{
    return bOverrideFieldOfView;
}

inline void UInventoryCameraComponent::SetFieldOfViewOverriden(const bool bNewOverriden)
{
    bOverrideFieldOfView = bNewOverriden;
}

inline float UInventoryCameraComponent::GetFieldOfViewOverride() const
{
    return FieldOfViewOverride;
}

inline void UInventoryCameraComponent::SetFieldOfViewOverride(const float NewFieldOfView)
{
    FieldOfViewOverride = FMath::Clamp(NewFieldOfView, 5.0f, 175.0f);
}

inline float UInventoryCameraComponent::GetPostProcessWeight() const
{
    return PostProcessWeight;
}

inline void UInventoryCameraComponent::SetPostProcessWeight(const float NewPostProcessWeight)
{
    PostProcessWeight = UInventoryMathLib::Clamp01(NewPostProcessWeight);
}

inline bool UInventoryCameraComponent::IsRightShoulder() const
{
    return bRightShoulder;
}

inline void UInventoryCameraComponent::SetRightShoulder(const bool bNewRightShoulder)
{
    bRightShoulder = bNewRightShoulder;
}
