#pragma once
#include "IVehicleState.h"
class BrakingState : public IVehicleState {
public:
    void update(Vehicle& vehicle, float deltaTime) override;
    const char* getName() const override { return "Braking"; }
};