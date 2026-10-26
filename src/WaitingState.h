#pragma once
#include "IVehicleState.h"
class WaitingState : public IVehicleState {
public:
    void update(Vehicle& vehicle, float deltaTime) override;
    const char* getName() const override { return "Waiting"; }
};