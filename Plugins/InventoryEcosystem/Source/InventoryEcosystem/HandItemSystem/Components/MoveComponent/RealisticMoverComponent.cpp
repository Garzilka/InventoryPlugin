#include "RealisticMoverComponent.h"
#include "InventoryEcosystem/HandItemSystem/Character/BaseCharacter.h"
#include "InventoryEcosystem/HandItemSystem/Data/LocomotionDefines.h"
#include "InventoryEcosystem/HandItemSystem/Objects/MoveMods/RealisticAirMode.h"
#include "InventoryEcosystem/HandItemSystem/Objects/MoveMods/RealisticWalkMode.h"

URealisticMoverComponent::URealisticMoverComponent()
{
    // Регистрируем режимы
    MovementModes.Add(RealisticModes::Walk, CreateDefaultSubobject<URealisticWalkMode>(TEXT("RealisticWalkMode")));
    MovementModes.Add(RealisticModes::Air, CreateDefaultSubobject<URealisticAirMode>(TEXT("RealisticAirMode")));

    // Стартовый режим
    StartingMovementMode = RealisticModes::Air;

    // Создаем настройки, чтобы они были видны в Blueprint
    RealisticSettings = CreateDefaultSubobject<URealisticMovementSettings>(TEXT("RealisticSettings"));
}
