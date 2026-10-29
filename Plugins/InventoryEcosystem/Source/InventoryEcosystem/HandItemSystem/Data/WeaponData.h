// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BalisticFXData.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraShakeBase.h"
#include "WeaponData.generated.h"

class ABaseWeaponModule;

namespace WeaponModificationType
{
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Muzzle);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Receiver);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FrontHandle);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Handle);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Optic);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Buttstock);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Magazine);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tactical);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Forearm);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Additional);
	HANDITEMSYSTEM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MuzzleAdd);
}

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWeaponFireType : uint8
{
	EWFT_None			= 0			UMETA(Hidden),
	EWFT_Single			= 1 << 0	UMETA(DisplayName = "Одиночный"),
	EWFT_Burst			= 1 << 1	UMETA(DisplayName = "Очередь"),
	EWFT_Auto			= 1 << 2	UMETA(DisplayName = "Авто"),
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWeaponModificationType : uint8
{
	EWMT_None				= 0			UMETA(DisplayName = "None"),
	EWMT_Ammo				= 1 << 0	UMETA(DisplayName = "Патроны"),
	EWMT_FireSound			= 1 << 1	UMETA(DisplayName = "Звук стрельбы"),
	EWMT_RecoilV			= 1 << 2	UMETA(DisplayName = "Вертикальная отдача"),
	EWMT_RecoilH			= 1 << 3	UMETA(DisplayName = "Горизонтальная отдача"),
	EWMT_Spread				= 1 << 4	UMETA(DisplayName = "Разброс"),
	EWMT_IK_Offset			= 1 << 5	UMETA(DisplayName = "IK Offset"),
	EWMT_OverrideJoinTarget	= 1 << 6	UMETA(DisplayName = "JoinTarget"),
};

USTRUCT(BlueprintType)
struct FWeaponModificateData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = "/Script/HandItemSystem.EWeaponModificationType", DisplayName = "Возможные действия"))
	int32 ActionMenu = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Звук одиночного выстрела",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_FireSound", EditConditionHides))
	USoundBase* OnShotFireSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Количество патрон",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_Ammo", EditConditionHides))
	int32 AmmoAmount = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Коэффициент вертикальной отдачи",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_RecoilV", EditConditionHides))
	float RecoilVMultiplayer = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Коэффициент вертикальной отдачи (Анимация)",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_RecoilV", EditConditionHides))
	float AnimRecoilVMultiplayer = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Коэффициент горизонтальной отдачи",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_RecoilH", EditConditionHides))
	float RecoilHMultiplayer = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Коэффициент горизонтальной отдачи (Анимация)",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_RecoilH", EditConditionHides))
	float AnimRecoilHMultiplayer = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "Коэффициент разброса",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_Spread", EditConditionHides))
	float SpreadMultiplayer = 1.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "IK Offset",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_IK_Offset", EditConditionHides))
	FTransform IK_Offset;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Эффекты", meta = (DisplayName = "JoinTargetLocation",
		EditCondition = "ActionMenu & EWeaponModificationType::EWMT_OverrideJoinTarget", EditConditionHides))
	FVector JoinTargetLocation;

};

USTRUCT(BlueprintType)
struct FWeaponModuleInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	bool AsMesh = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceInlineRow))
	TMap<FName, FName> SocketAttach;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Меш", EditCondition = "AsMesh", EditConditionHides))
	USkeletalMesh* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Класс модуля", EditCondition = "!AsMesh", EditConditionHides))
	TSubclassOf<ABaseWeaponModule> Mod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Модификация"))
	FWeaponModificateData ModData;

};

USTRUCT(BlueprintType)
struct FWeaponSocketInfo
{
	GENERATED_BODY()
    
	FWeaponSocketInfo () = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AttachTag;
	
	UPROPERTY(BlueprintReadOnly)
	class UBaseWeaponModuleService* AttachedModule;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName UID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Socket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Смещение точки установки", ToolTip="Смещение точки появления виджета под установку предмета"))
	FVector CellOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Смещение точки указателя", ToolTip="Смещение точки указателя от куда будет отображаться стрелка"))
	FVector TargetRowOffset;

	friend bool operator==(const FWeaponSocketInfo& Left, const FWeaponSocketInfo& Right)
	{
		return (Left.AttachTag == Right.AttachTag && Left.Socket == Right.Socket && (Left.UID == NAME_None || Left.UID == Right.UID));
	}
};

USTRUCT(BlueprintType)
struct FRecoilCamera
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Тряска камеры"))
	TSubclassOf<UCameraShakeBase> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Вертикальная отдача"))
	float VerticalRecoil = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Коэфициент вертикальной отдачи"))
	float VerticalRecoilCoef = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Горизонтальная отдача"))
	FVector2D HorizontalRecoil= FVector2D(-0.25f, 0.25f);
};

USTRUCT(BlueprintType)
struct FRecoilAnimation
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Кривая смещения при стрельбе"))
	UCurveVector* RecoilLocationCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Рандомная цифра домножения смещения"))
	FVector2D RecoilLocationRandomness = FVector2D(1.0f, 2.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Кривая поворота при стрельбе"))
	UCurveVector* RecoilRotationCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Рандомная цифра домножения поворота Вперед/Назад"))
	FVector2D RecoilPitchRandomness = FVector2D(1.0f, 2.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Рандомная цифра домножения поворота набок Влево/Вправо"))
	FVector2D RecoilRollRandomness = FVector2D(-3.0f, 3.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Отдача анимация|Отдача анимация", meta = (DisplayName = "Рандомная цифра домножения поворота Влево/Вправо"))
	FVector2D RecoilYawRandomness = FVector2D(-8.0f, 8.0f);
};

USTRUCT(BlueprintType)
struct FRecoilSpread
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Сила разброса"))
	UCurveFloat* SpreadPower = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Максимальный разборос при выстреле:"))
	int32 MaxSpreadOnShot = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Разброс по вертикали и горизонтали"))
	FVector2D Spread = FVector2D(0.5f, 0.5f);
};

USTRUCT(BlueprintType)
struct FRecoilData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Отдача камера"))
	FRecoilCamera RecoilCameraData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Отдача анимация"))
	FRecoilAnimation RecoilAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Разброс"))
	FRecoilSpread RecoilSpreadData;

	FVector GetFixedLocationVector(const float Time) const
	{
		if (RecoilAnimationData.RecoilLocationCurve)
		{
			return RecoilAnimationData.RecoilLocationCurve->GetVectorValue(Time);
		}
		return FVector::ZeroVector;
	}

	FVector GetFixedRotationVector(const float Time) const
	{
		if (RecoilAnimationData.RecoilRotationCurve)
		{
			return RecoilAnimationData.RecoilRotationCurve->GetVectorValue(Time);
		}
		return FVector::ZeroVector;
	}
};

USTRUCT(BlueprintType)
struct FWeaponRepModData
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintReadOnly)
	class UBaseWeaponModuleService* ObjectModuleInfo = nullptr;

	UPROPERTY(BlueprintReadOnly)
	AActor* ActorModuleInfo = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FName SocketAttach = FName(NAME_None);


	friend bool operator== (const FWeaponRepModData& Left, const FWeaponRepModData& Right)
	{
		return (Left.ActorModuleInfo == Right.ActorModuleInfo && Left.SocketAttach == Right.SocketAttach);
	}

	friend bool operator== (const FWeaponRepModData& Left, const AActor* Right)
	{
		return (Left.ActorModuleInfo == Right);
	}

	friend bool operator== (const FWeaponRepModData& Left, const UBaseWeaponModuleService* Right)
	{
		return (Left.ObjectModuleInfo == Right);
	}
};

USTRUCT(BlueprintType)
struct FFirearmStats
{
	GENERATED_BODY()
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Эргономика", meta = (DisplayName = "Вес"))
	float Weight = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Эргономика", meta = (DisplayName = "Эргономика"))
	float Ergonomics = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Эргономика", meta = (DisplayName = "Усиление отдачи"))
	float RecoilMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeaponSettingControll
{
	GENERATED_BODY()
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (AllowPrivateAccess = "true", DisplayName = "Коэфициент разброса"))
	float SpreedCoef = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (AllowPrivateAccess = "true", DisplayName = "Коэфициент вертикальной отдачи (Камера)"))
	float VericalRecoilCoefCam = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (AllowPrivateAccess = "true", DisplayName = "Коэфициент горизонтальной отдачи (Камера)"))
	float HorizontalRecoilCoefCam = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (AllowPrivateAccess = "true", DisplayName = "Коэфициент вертикальной отдачи (Камера)"))
	float VericalRecoilCoefAnim = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (AllowPrivateAccess = "true", DisplayName = "Коэфициент горизонтальной отдачи (Камера)"))
	float HorizontalRecoilCoefAnim = 1.0f;

	FWeaponSettingControll operator*(FWeaponSettingControll Item)
	{
		SpreedCoef *= Item.SpreedCoef;
		VericalRecoilCoefCam *= Item.VericalRecoilCoefCam;
		HorizontalRecoilCoefCam *= Item.HorizontalRecoilCoefCam;
		VericalRecoilCoefAnim *= Item.VericalRecoilCoefAnim;
		VericalRecoilCoefAnim *= Item.VericalRecoilCoefAnim;
		return *this;
	}
};

UCLASS(BlueprintType)
class HANDITEMSYSTEM_API UBaseWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Рендер оружия", meta = (DisplayName = "Орфографическая ширина"))
	float OrtoWidth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Рендер оружия", meta = (DisplayName = "Дистанция до оружия"))
	float TargetArmLength = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Рендер оружия", meta = (DisplayName = "Смещение камеры"))
	FVector RenderOffsetLocationMesh = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Рендер оружия", meta = (DisplayName = "Смещение камеры"))
	FVector RenderOffsetLocationCamera = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Рендер оружия", meta = (DisplayName = "Поворот камеры"))
	FRotator RenderOffsetRotationCamera = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Вид", meta = (DisplayName = "Смещение меша"))
	FVector LocationMeshOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Вид", meta = (DisplayName = "Поворот меша"))
	FRotator RotationMeshOffset = FRotator::ZeroRotator;

};

USTRUCT(BlueprintType)
struct FAimCameraSettings
{
	GENERATED_BODY()
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraFOVZoom = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraFOVZoomSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraDistance = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUsedFixedCameraDistance = false;

	FAimCameraSettings()
	{

	};
};

UCLASS(BlueprintType)
class HANDITEMSYSTEM_API UBaseFireWeaponData : public UBaseWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Тактический патрон",
		ToolTip = "Может ли в дуле остаться патрон. Например при перезарядке оружия с внешним магазином."))
	bool bHasTacticalAmmo = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики")
	bool UseOffsetDebug = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики")
	FTransform DebugIKOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики")
	bool UseJoinDebug = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики")
	FVector DebugJoin;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Использование IK"))
	bool bIsUseIK = true;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Использование IK"))
	FVector JoinTargetLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Базовое смещение"))
	FTransform DefaultPoseOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Базовое смещение TPS"))
	FTransform DefaultPoseOffsetTPS;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Смещение прицеливания"))
	FTransform AIM_PoseOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции", meta = (DisplayName = "Смещение прицеливания TPS"))
	FTransform AIM_PoseOffsetTPS;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции")
	FTransform MovePoseOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики|Позиции")
	FTransform HighPortPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики|Камера")
	FAimCameraSettings CameraSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Максимальное количество патрон",
		ToolTip = "Используется для оружия с внутреннем магазином"))
	int32 MaxAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Тактико технические характеристики", meta = (Bitmask, BitmaskEnum = "/Script/HandItemSystem.EWeaponFireType", DisplayName = "Поддерживаемые типы стрельбы"))
	int32 FireTypes = 0; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (EditCondition = "FireTypes & EWeaponFireType::EWFT_Burst", EditConditionHides, DisplayName = "Максимальное количество патрон"))
	int32 AmountPerBurst = 0;

	UPROPERTY(EditAnywhere, Category = "Тактико технические характеристики", meta = (DisplayName = "Тип стрельбы по умолчанию"))
	EWeaponFireType FireTypeByDefault = EWeaponFireType::EWFT_Auto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Выстрелы в минуту"))
	float UseRateDelay = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Дальность выстрела"))
	float FireDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Использование дроби"))
	bool bIsPellets = false;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики", 
		meta = (EditCondition = "bIsPellets", EditConditionHides, DisplayName = "Количество дробинок"))
	int32 AmountPellets = false;

	UPROPERTY(EditDefaultsOnly, Category = "Тактико технические характеристики",
		meta = (EditCondition = "bIsPellets", EditConditionHides, DisplayName = "Угол разлета дроби"))
	float AnglePelletsSpread = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Тактико технические характеристики")
	float DefaultAimSwayMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Прицеливание", meta = (DisplayName = "Скорость перехода в FOV"))
	float SpeedInterpolateFOVIn = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Прицеливание", meta = (DisplayName = "Скорость перехода из FOV"))
	float SpeedInterpolateFOVFrom = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Прицеливание", meta = (DisplayName = "Fov при прицеливании"))
	float FOVAiming = 70.f;

	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Анимационный класс"))
	TSubclassOf<UAnimInstance> WeaponAnimClass;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Анимация выстрела персонаж"))
	UAnimMontage* CharacterFireAnimated;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Анимация выстрела оружия"))
	UAnimMontage* WeaponFireAnimated;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Цикличная зарядка"))
	bool bIsCircleReload = false;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Цикличная зарядка"))
	FName SocketReatachReload = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "!bIsCircleReload", EditConditionHides, DisplayName = "Анимация перезарядки персонаж"))
	UAnimMontage* CharacterFullReload;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "!bIsCircleReload", EditConditionHides, DisplayName = "Анимация перезарядки оружие"))
	UAnimMontage* WeaponFullReload;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "!bIsCircleReload", EditConditionHides, DisplayName = "Анимация смены магазина персонаж"))
	UAnimMontage* CharacterHotReload;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "!bIsCircleReload", EditConditionHides, DisplayName = "Анимация смены магазина оружие"))
	UAnimMontage* WeaponHotReload;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки персонаж старт"))
	UAnimMontage* CharacterCircleChargeStart;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки оружие старт"))
	UAnimMontage* WeaponCircleChargeStart;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки персонаж цикл"))
	UAnimMontage* CharacterCircleChargeLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки оружие цикл"))
	UAnimMontage* WeaponCircleChargeLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки персонаж выход"))
	UAnimMontage* CharacterCircleChargeExit;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", 
		meta = (EditCondition = "bIsCircleReload", EditConditionHides, DisplayName = "Анимация дозарядки оружие выход"))
	UAnimMontage* WeaponCircleChargeExit;
	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Сокет Muzzle"))
	FName MuzzleSocket = "SKT_Muzzle";
	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Сокет AmmoExit"))
	FName SocketAmmoExit = "SKT_AmmoExit";
	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Звук смены типа стрельбы"))
	USoundBase* ChangeFireMode;
	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Звук пустого оружия"))
	USoundBase* EmptySound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Звук одиночного выстрела"))
	USoundBase* OnShotFireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Эффект выстрела"))
	UParticleSystem* FireEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Эффекты", meta = (DisplayName = "Эффект вылета гильзы"))
	UParticleSystem* AmmoExit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (DisplayName = "Настройки отдачи"))
	FRecoilData RecoilSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (DisplayName = "Параметры оружия влияют на персонажа"))
	bool ControlMovementSwayByStats = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (DisplayName = "Кривая покачивания оружия"))
	UCurveVector* MovementSwayCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Контроль", meta = (DisplayName = "Статистика оружия"))
	FFirearmStats FirearmStats;

};

UCLASS(BlueprintType)
class HANDITEMSYSTEM_API UBaseMeleeData : public UBaseWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Урон"))
	float Damage = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Дистанция нанесения урона"))
	float Distance = 75.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Тактико технические характеристики", meta = (DisplayName = "Урон"))
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Вид", meta = (DisplayName = "BalisticFX"))
	UBalisticFXDataTable* BalisticFXData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Косметика|Вид", meta = (DisplayName = "Меш объекта"))
	UStaticMesh* WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Косметика|Анимация", meta = (DisplayName = "Быстрый удар"))
	TArray<UAnimMontage*> AttackAnimations;

};
