#pragma once

#include "CoreMinimal.h"
#include "MovementMode.h"
#include "RealisticWalkMode.generated.h"

UCLASS()
class HANDITEMSYSTEM_API URealisticWalkMode : public UBaseMovementMode
{
    GENERATED_BODY()
public:
    URealisticWalkMode();

    virtual void GenerateMove_Implementation(const FMoverSimContext& SimContext, const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, UPARAM(ref) FProposedMove& OutProposedMove) const override;
    virtual void SimulationTick_Implementation(const FSimulationTickParams& Params, FMoverTickEndData& OutputState) override;
};