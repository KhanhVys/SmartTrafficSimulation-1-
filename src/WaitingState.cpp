#include "WaitingState.h"
#include "Vehicle.h"
#include "AcceleratingState.h"

void WaitingState::update(Vehicle& v, float dt) {
    (void)dt;
    if (v.canProceed() && v.isSafeToMove()) {
        v.setState(std::make_unique<AcceleratingState>());
    }
}