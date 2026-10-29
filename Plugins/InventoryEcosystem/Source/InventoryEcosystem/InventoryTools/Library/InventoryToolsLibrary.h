// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InventoryToolsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYTOOLS_API UInventoryToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    static constexpr auto DrawImpactPointSize{32.0f};
    static constexpr auto DrawLineThickness{1.0f};
    static constexpr auto DrawArrowSize{50.0f};
    static constexpr auto DrawCircleSidesCount{16};

    UFUNCTION(BlueprintCallable, Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, Rotation, Color"))
    static void DrawDebugSphereAlternative(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation,
                                           float Radius, const FLinearColor& Color, float Duration = 0.0f,
                                           float Thickness = 1.0f, uint8 DepthPriority = 0);
    
    UFUNCTION(BlueprintPure, Meta = (DefaultToSelf = "Actor", AutoCreateRefTerm = "DisplayName", ReturnDisplayName = "Value"))
    static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);
    
    UFUNCTION(BlueprintCallable, Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, Color"))
    static void DrawDebugSweepSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
                                     const FLinearColor& Color, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

    static float GetTimeToNotify(UAnimMontage* FromMontage, TSubclassOf<UAnimNotify> Notify, bool WithSclae = true);
    static float GetTimeToNotify(UAnimMontage* FromMontage, FName NameNotify, bool WithSclae = true);
    static bool CheckDotAngle(FVector From, FVector To, FRotator DotCheck, float MaxAngle);

    static bool IsInGameView();

    static FName GetSimpleTagName(const FGameplayTag& Tag);
    
    template <class T>
    FORCEINLINE static T* GetComponentFromOwner(AActor* From, TSubclassOf<UActorComponent> ClassComponent)
    {
    	if (From == nullptr) return nullptr;
    	if (UActorComponent* possibleComponent = From->GetComponentByClass(ClassComponent))
    	{
    		if (possibleComponent)
    		{
    			return Cast<T>(possibleComponent);
    		}
    	}
    	if (APlayerController* possibleOwner = Cast<APlayerController>(From))
    	{
    		UActorComponent* possibleComponent = possibleOwner->GetComponentByClass(ClassComponent);
    		if (possibleComponent)
    		{
    			return Cast<T>(possibleComponent);
    		}
    		if (possibleOwner->GetPawn() == nullptr) return nullptr;
    		possibleComponent = possibleOwner->GetPawn()->GetComponentByClass(ClassComponent);
    		if (possibleComponent)
    		{
    			return Cast<T>(possibleComponent);
    		}
    	}
    	if (APawn* possibleOwner = Cast<APawn>(From))
    	{
    		UActorComponent* possibleComponent = possibleOwner->GetComponentByClass(ClassComponent);
    		if (possibleComponent)
    		{
    			return Cast<T>(possibleComponent);
    		}
    		if (possibleOwner->GetController() == nullptr) return nullptr;
    		possibleComponent = possibleOwner->GetController()->GetComponentByClass(ClassComponent);
    		if (possibleComponent)
    		{
    			return Cast<T>(possibleComponent);
    		}
    	}
    	return nullptr;
    }
};
