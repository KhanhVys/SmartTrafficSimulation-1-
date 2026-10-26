#include "AcceleratingState.h"
#include "Vehicle.h"
#include "CruisingState.h"
#include "BrakingState.h"

void AcceleratingState::update(Vehicle& v, float dt) {
    if (v.shouldBrake()) {
        v.setState(std::make_unique<BrakingState>());
        return;
    }
    v.setSpeed(v.getSpeed() + v.getAcceleration() * dt);
    if (v.getSpeed() >= v.getMaxSpeed()) {
        v.setSpeed(v.getMaxSpeed());
        v.setState(std::make_unique<CruisingState>());
        return;
    }
    v.moveForward(dt);
}