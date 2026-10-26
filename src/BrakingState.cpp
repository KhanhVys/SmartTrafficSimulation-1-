#include "BrakingState.h"
#include "Vehicle.h"
#include "WaitingState.h"
#include "AcceleratingState.h"

void BrakingState::update(Vehicle& v, float dt) {
    if (!v.shouldBrake()) {
        v.setState(std::make_unique<AcceleratingState>());
        return;
    }
    v.setSpeed(v.getSpeed() - v.getBraking() * dt);
    if (v.getSpeed() <= 0.0f) {
        v.setSpeed(0.0f);
        v.setState(std::make_unique<WaitingState>());
        return;
    }
    v.moveForward(dt);
}