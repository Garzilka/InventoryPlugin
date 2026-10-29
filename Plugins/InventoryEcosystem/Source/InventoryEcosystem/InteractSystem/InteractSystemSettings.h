#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "InteractSystemSettings.generated.h"


UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Система взаимодействия"))
class INTERACTSYSTEM_API UInteractSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/** Меш предпросмотра для проверки точки взаимодействия */
	UPROPERTY(Config, EditAnywhere, Category = "Система взаимодействия", meta = (DisplayName = "Меш предпросмотра"))
	TSoftObjectPtr<USkeletalMesh> PreviewInteractMesh;

	/** Таблица пресетов взаимодействия */
	UPROPERTY(Config, EditAnywhere, Category = "Система взаимодействия", meta = (DisplayName = "Пресеты взаимодействий"))
	TSoftObjectPtr<UDataTable> InteractPresets;

	/** Профиль коллизии объектов, с которыми можно взаимодействовать */
	UPROPERTY(Config, EditAnywhere, Category = "Система взаимодействия", meta = (DisplayName = "Профиль коллизии взаимодействия"))
	FName InteractProfile = FName("Interacted");


	static const UInteractSystemSettings* Get();
};