#pragma once

#include "CoreMinimal.h"
#include "MoverComponent.h"
// Важный инклюд, чтобы видеть настройки
#include "InventoryEcosystem/HandItemSystem/Objects/MoveSettings/RealisticMovementSettings.h" 
#include "RealisticMoverComponent.generated.h"

UCLASS(ClassGroup = (Mover), meta = (BlueprintSpawnableComponent))
class HANDITEMSYSTEM_API URealisticMoverComponent : public UMoverComponent
{
    GENERATED_BODY()

public:
    URealisticMoverComponent();

    UPROPERTY(EditAnywhere, Instanced, Category = "Realistic Movement")
    TObjectPtr<URealisticMovementSettings> RealisticSettings;

};