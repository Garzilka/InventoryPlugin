// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponScope.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
ABaseWeaponScope::ABaseWeaponScope()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent0"));
	
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCapture->SetupAttachment(RootComponent);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	//SceneCapture->bHiddenInGame = true;           
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->bAlwaysPersistRenderingState = true;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;

	SceneCapture->ShowFlags.DynamicShadows = true;
	SceneCapture->ShowFlags.AmbientOcclusion = false;
	SceneCapture->ShowFlags.AmbientCubemap = false;
	SceneCapture->ShowFlags.DistanceFieldAO = false;
	SceneCapture->ShowFlags.LightFunctions = false;
	SceneCapture->ShowFlags.LightShafts = false;
	SceneCapture->ShowFlags.ReflectionEnvironment = false;
	SceneCapture->ShowFlags.ScreenSpaceReflections = false;
	SceneCapture->ShowFlags.TexturedLightProfiles = false;
	SceneCapture->ShowFlags.VolumetricFog = false;
	SceneCapture->ShowFlags.MotionBlur = 0;
}

void ABaseWeaponScope::SetActiveRender(bool IsActive)
{
	if (IsActive && HasOptic)
	{
		SceneCapture->Activate(true);
		SetActorTickInterval(1 / 60.f);
		SceneCapture->bCaptureEveryFrame = false;
		SceneCapture->bCaptureOnMovement = true;
	}
	else
	{
		SceneCapture->bCaptureEveryFrame = false;
		SceneCapture->bCaptureOnMovement = false;
		SceneCapture->Deactivate();
		SetActorTickInterval(1 / 5.f);
	}
	SetActorTickEnabled(IsActive && HasOptic);
}

void ABaseWeaponScope::BeginPlay()
{
	Super::BeginPlay();
	SetActiveRender(false);
}