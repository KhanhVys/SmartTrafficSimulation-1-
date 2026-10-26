#include "CruisingState.h"
#include "Vehicle.h"
#include "BrakingState.h"
#include "TurningState.h"

void CruisingState::update(Vehicle& v, float dt) {
    if (v.shouldBrake()) {
        v.setState(std::make_unique<BrakingState>());
        return;
    }
    if (v.shouldTurn()) {
        v.setState(std::make_unique<TurningState>());
        return;
    }
    v.moveForward(dt);
}