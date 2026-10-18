#include "Vehicle.h"
#include <cmath>

Vehicle::Vehicle(int id, Direction dir, Movement move, int lane, double maxSpd, double startPos)
    : vehicleId(id), direction(dir), movement(move), laneIndex(lane),
      speed(0.0), maxSpeed(maxSpd), acceleration(8.0), deceleration(12.0),
      position(startPos), x(0.0), y(0.0) {
    currentState = std::make_unique<AcceleratingState>();
}

void Vehicle::setState(std::unique_ptr<IVehicleState> newState) {
    if (newState && (!currentState || currentState->getStateName() != newState->getStateName())) {
        currentState = std::move(newState);
    }
}

void Vehicle::update(double dt, double distanceToStopLine, bool isRedLight, bool clearAhead) {
    position = distanceToStopLine;

    bool effectiveRedLight = isRedLight && (position > 0.0);

    double brakingDist = (speed * speed) / (2.0 * deceleration);
    
    bool needToBrakeForLight = effectiveRedLight && (position <= brakingDist);

    if (needToBrakeForLight || !clearAhead) {
        if (speed > 0.0) {
            setState(std::make_unique<BrakingState>());
        } else {
            setState(std::make_unique<WaitingState>());
        }
    } else {
        if (position <= 0.0 && (movement == Movement::Left || movement == Movement::Right || movement == Movement::UTurn)) {
             setState(std::make_unique<TurningState>());
        } else if (speed <= 0.0) {
            setState(std::make_unique<AcceleratingState>());
        } else if (speed >= maxSpeed) {
            setState(std::make_unique<CruisingState>());
        } else if (getStateName() == "Waiting") {
            setState(std::make_unique<AcceleratingState>());
        }
    }

    if (speed > 0.0) {
        position -= speed * dt;
        
        if (effectiveRedLight && position < 0.0) {
            position = 0.0;
            speed = 0.0;
            setState(std::make_unique<WaitingState>());
        }

        if (position < -60.0) {
            position = 180.0; 
        }
    }

    if (direction == Direction::North) {
        x = laneIndex * 3.5; 
        y = -position;
    } else if (direction == Direction::South) {
        x = -laneIndex * 3.5;
        y = position;
    } else if (direction == Direction::East) {
        x = -position;
        y = -laneIndex * 3.5;
    } else if (direction == Direction::West) {
        x = position;
        y = laneIndex * 3.5;
    }

    if (currentState) {
        currentState->handle(*this, dt);
    }
}
