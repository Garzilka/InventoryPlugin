// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryMathLib.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYTOOLS_API UInventoryMathLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|float", Meta = (ReturnDisplayName = "Value"))
    static float Clamp01(float Value);

    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|float", Meta = (ReturnDisplayName = "Interpolation Ammount"))
    static float ExponentialDecay(float DeltaTime, float Lambda);

    template <typename ValueType>
    static ValueType ExponentialDecay(const ValueType& Current, const ValueType& Target, float DeltaTime, float Lambda);

    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Ротаторы", Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotator"))
    static FRotator ExponentialDecayRotator(const FRotator& Current, const FRotator& Target, float DeltaTime, float Lambda);
	
    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Вектора", Meta = (ReturnDisplayName = "Direction"))
    static FVector AngleToDirectionXY(float Angle);

    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Трансформы", Meta = (ReturnDisplayName = "RelativeTransform"))
    static FTransform Lib_GetRelativeTransformReverse(FTransform At, FTransform Offset);

    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Трансформы", Meta = (ReturnDisplayName = "RelativeTransform"))
    static FTransform Lib_GetRelativeTransform(FTransform At, FTransform Offset);

    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Вектора", Meta = (ReturnDisplayName = "Direction"))
    static FVector RadianToDirectionXY(const float Radian);
    
    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Вектора", DisplayName = "Clamp Magnitude 01 2D",
    Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector2D ClampMagnitude012D(const FVector2D& Vector);
    
    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Вектора", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector PerpendicularCounterClockwiseXY(const FVector& Vector);
    
    UFUNCTION(BlueprintPure, Category = "Инвентарь|Математика|Вектора", Meta = (AutoCreateRefTerm = "Vector", ReturnDisplayName = "Vector"))
    static FVector2D ConvertWorldToScreen(APlayerController* PC, const FVector& WorldLocation, USceneCaptureComponent2D* AtCaptureComponent, float& OutDepth);
};

inline float UInventoryMathLib::Clamp01(const float Value)
{
    return Value <= 0.0f
                   ? 0.0f
                   : Value >= 1.0f
                   ? 1.0f
                   : Value;
}
inline float UInventoryMathLib::ExponentialDecay(const float DeltaTime, const float Lambda)
{
    return 1.0f - FMath::InvExpApprox(Lambda * DeltaTime);
}

template <typename ValueType>
ValueType UInventoryMathLib::ExponentialDecay(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float Lambda)
{
    return Lambda > 0.0f
                   ? FMath::Lerp(Current, Target, ExponentialDecay(DeltaTime, Lambda))
                   : Target;
}

inline FRotator UInventoryMathLib::ExponentialDecayRotator(const FRotator& Current, const FRotator& Target,
                                                  const float DeltaTime, const float Lambda)
{
	return ExponentialDecay(Current, Target, DeltaTime, Lambda);
}

inline FVector UInventoryMathLib::AngleToDirectionXY(const float Angle)
{
	return RadianToDirectionXY(FMath::DegreesToRadians(Angle));
}

inline FTransform UInventoryMathLib::Lib_GetRelativeTransformReverse(FTransform At, FTransform Offset)
{
	return Offset.GetRelativeTransformReverse(At);
}
inline FTransform UInventoryMathLib::Lib_GetRelativeTransform(FTransform At, FTransform Offset)
{
	return Offset.GetRelativeTransform(At);
}

inline FVector UInventoryMathLib::RadianToDirectionXY(const float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return { Cos, Sin, 0.0f };
}

inline FVector2D UInventoryMathLib::ClampMagnitude012D(const FVector2D& Vector)
{
	const auto MagnitudeSquared{ Vector.SizeSquared() };

	if (MagnitudeSquared <= 1.0f)
	{
		return Vector;
	}

	const auto Scale{ FMath::InvSqrt(MagnitudeSquared) };

	return { Vector.X * Scale, Vector.Y * Scale };
}

inline FVector UInventoryMathLib::PerpendicularCounterClockwiseXY(const FVector& Vector)
{
	return { -Vector.Y, Vector.X, Vector.Z };
}

inline FVector2D UInventoryMathLib::ConvertWorldToScreen(APlayerController* PC, const FVector& WorldLocation, USceneCaptureComponent2D* AtCaptureComponent, float& OutDepth)
{
	FMinimalViewInfo Info;
	AtCaptureComponent->GetCameraView(0.f, Info);
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	FMatrix ViewMatrix;
	UGameplayStatics::GetViewProjectionMatrix(Info, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);
	
	// Преобразуем WorldLocation в координаты камеры (View Space)
	FVector4 ViewSpacePos = ViewMatrix.TransformFVector4(FVector4(WorldLocation, 1.0f));

	// Применяем Projection Matrix
	FVector4 ClipSpacePos = ProjectionMatrix.TransformFVector4(ViewSpacePos);

	// Проверка на деление на нулевой W
	if (ClipSpacePos.W == 0.0f)
	{
		return FVector2D::ZeroVector;
	}

	// Нормализация в Normalized Device Coordinates (NDC)
	FVector4 NDC;
	NDC.X = ClipSpacePos.X / ClipSpacePos.W;
	NDC.Y = ClipSpacePos.Y / ClipSpacePos.W;
	NDC.Z = ClipSpacePos.Z / ClipSpacePos.W;
	// NDC.Z обычно в диапазоне [-1, 1], можно вернуть глубину как NDC.Z

	// Проверка видимости: NDC.x,y должны быть в [-1, 1]
	if (NDC.X < -1.0f || NDC.X > 1.0f || NDC.Y < -1.0f || NDC.Y > 1.0f)
	{
		return FVector2D::ZeroVector;
	}

	// Преобразование из NDC в экранные координаты в пикселях
	// Получаем размер Viewport
	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);

	// Преобразование: X_screen = ((NDC.X + 1) * 0.5) * Width
	// Y_screen = ((1 - NDC.Y) * 0.5) * Height  (учитывая, что Y растёт вниз)
	float ScreenX = ((NDC.X + 1.0f) * 0.5f) * (float)ViewportX;
	float ScreenY = ((1.0f - NDC.Y) * 0.5f) * (float)ViewportY;


	OutDepth = NDC.Z; // можно хранить глубину в диапазоне [-1,1]
	return FVector2D(ScreenX, ScreenY);
}
