#include "Vehicle.h"

Vehicle::Vehicle(std::string id, Direction dir, TurnType turn, int lane, double maxSpd)
    : id(id), direction(dir), turnType(turn), laneIndex(lane),
      speed(0.0), maxSpeed(maxSpd), acceleration(8.0), deceleration(12.0),
      position(150.0) {
    currentState = std::make_unique<AcceleratingState>();
}

void Vehicle::setState(std::unique_ptr<IVehicleState> newState) {
    if (newState && (!currentState || currentState->getStateName() != newState->getStateName())) {
        currentState = std::move(newState);
    }
}

void Vehicle::update(double dt, double distanceToStopLine, bool isRedLight, bool clearAhead) {
    position = distanceToStopLine;

    if ((isRedLight && position > 0.0 && position <= 40.0) || !clearAhead) {
        if (speed > 0.0) {
            setState(std::make_unique<BrakingState>());
        } else {
            setState(std::make_unique<WaitingState>());
        }
    } else if (!isRedLight && clearAhead) {
        if (speed <= 0.0) {
            setState(std::make_unique<AcceleratingState>());
        } else if (speed >= maxSpeed) {
            setState(std::make_unique<CruisingState>());
        } else if (getStateName() == "Waiting") {
            setState(std::make_unique<AcceleratingState>());
        }
    }

    if (speed > 0.0 && position > 0.0) {
        position -= speed * dt;
        if (position < 0.0) position = 0.0;
    }

    if (currentState) {
        currentState->handle(*this, dt);
    }
}