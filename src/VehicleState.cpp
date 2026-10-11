#include "VehicleState.h"
#include "Vehicle.h"
#include <algorithm>

void CruisingState::handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) {
    if ((isRedLight && distanceToNext < 25.0) || distanceToNext < vehicle.getSafeDistance()) {
        vehicle.setState(std::make_shared<BrakingState>());
    } else {
        vehicle.move(dt);
    }
}

void BrakingState::handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) {
    double newSpeed = vehicle.getSpeed() - vehicle.getDeceleration() * dt;
    if (newSpeed <= 0.0) {
        vehicle.setSpeed(0.0);
        vehicle.setState(std::make_shared<WaitingState>());
    } else {
        vehicle.setSpeed(newSpeed);
        vehicle.move(dt);
        if (!isRedLight && distanceToNext > vehicle.getSafeDistance() * 1.5) {
            vehicle.setState(std::make_shared<AcceleratingState>());
        }
    }
}

void WaitingState::handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) {
    vehicle.setSpeed(0.0);
    if (!isRedLight && distanceToNext > vehicle.getSafeDistance()) {
        vehicle.setState(std::make_shared<AcceleratingState>());
    }
}

void TurningState::handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) {
    vehicle.setSpeed(5.0); 
    vehicle.move(dt);
}

void AcceleratingState::handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) {
    if ((isRedLight && distanceToNext < 25.0) || distanceToNext < vehicle.getSafeDistance()) {
        vehicle.setState(std::make_shared<BrakingState>());
        return;
    }
    double newSpeed = vehicle.getSpeed() + vehicle.getAcceleration() * dt;
    if (newSpeed >= vehicle.getMaxSpeed()) {
        vehicle.setSpeed(vehicle.getMaxSpeed());
        vehicle.setState(std::make_shared<CruisingState>());
    } else {
        vehicle.setSpeed(newSpeed);
    }
    vehicle.move(dt);
}