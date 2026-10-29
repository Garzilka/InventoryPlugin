// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CameraConstants.generated.h"

/**
 * 
 */
UCLASS()
class HANDITEMSYSTEM_API UCameraConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Animation Curves

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FovOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagXCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagYCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagZCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationLagCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FirstPersonOverrideCurveName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& TraceOverrideCurveName();
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& RootBoneName();
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraCurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "Inventory|Camera Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraTracesDebugDisplayName();
};

inline const FName& UCameraConstants::HeadBoneName()
{
	static const FName Name{TEXTVIEW("head")};
	return Name;
}

inline const FName& UCameraConstants::RootBoneName()
{
	static const FName Name{TEXTVIEW("root")};
	return Name;
}

inline const FName& UCameraConstants::CameraOffsetXCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetX")};
	return Name;
}

inline const FName& UCameraConstants::CameraOffsetYCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetY")};
	return Name;
}

inline const FName& UCameraConstants::CameraOffsetZCurveName()
{
	static const FName Name{TEXTVIEW("CameraOffsetZ")};
	return Name;
}

inline const FName& UCameraConstants::FovOffsetCurveName()
{
	static const FName Name{TEXTVIEW("FovOffset")};
	return Name;
}

inline const FName& UCameraConstants::PivotOffsetXCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetX")};
	return Name;
}

inline const FName& UCameraConstants::PivotOffsetYCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetY")};
	return Name;
}

inline const FName& UCameraConstants::PivotOffsetZCurveName()
{
	static const FName Name{TEXTVIEW("PivotOffsetZ")};
	return Name;
}

inline const FName& UCameraConstants::LocationLagXCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagX")};
	return Name;
}

inline const FName& UCameraConstants::LocationLagYCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagY")};
	return Name;
}

inline const FName& UCameraConstants::LocationLagZCurveName()
{
	static const FName Name{TEXTVIEW("LocationLagZ")};
	return Name;
}

inline const FName& UCameraConstants::RotationLagCurveName()
{
	static const FName Name{TEXTVIEW("RotationLag")};
	return Name;
}

inline const FName& UCameraConstants::FirstPersonOverrideCurveName()
{
	static const FName Name{TEXTVIEW("FirstPersonOverride")};
	return Name;
}

inline const FName& UCameraConstants::TraceOverrideCurveName()
{
	static const FName Name{TEXTVIEW("TraceOverride")};
	return Name;
}

inline const FName& UCameraConstants::CameraCurvesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Inventory.CameraCurves")};
	return Name;
}

inline const FName& UCameraConstants::CameraShapesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Inventory.CameraShapes")};
	return Name;
}

inline const FName& UCameraConstants::CameraTracesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("Inventory.CameraTraces")};
	return Name;
}
