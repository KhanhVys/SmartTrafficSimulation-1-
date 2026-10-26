#include "TurningState.h"
#include "Vehicle.h"
#include "CruisingState.h"

void TurningState::update(Vehicle& v, float dt) {
    v.updateTurning(dt);
    if (v.isTurnComplete()) {
        v.setState(std::make_unique<CruisingState>());
    }
}