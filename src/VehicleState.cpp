#include "IVehicleState.h"
#include "Vehicle.h"

void AcceleratingState::handle(Vehicle& vehicle, double dt) {
    double speed = vehicle.getSpeed();
    if (speed < vehicle.getMaxSpeed()) {
        speed += vehicle.getAcceleration() * dt;
        if (speed > vehicle.getMaxSpeed()) speed = vehicle.getMaxSpeed();
        vehicle.setSpeed(speed);
    }
}

void CruisingState::handle(Vehicle& vehicle, double dt) {
    vehicle.setSpeed(vehicle.getMaxSpeed());
}

void BrakingState::handle(Vehicle& vehicle, double dt) {
    double speed = vehicle.getSpeed();
    if (speed > 0.0) {
        speed -= vehicle.getDeceleration() * dt;
        if (speed < 0.0) speed = 0.0;
        vehicle.setSpeed(speed);
    }
}

void WaitingState::handle(Vehicle& vehicle, double dt) {
    vehicle.setSpeed(0.0);
}

void TurningState::handle(Vehicle& vehicle, double dt) {
    const double turningSpeed = 15.0;
    vehicle.setSpeed(turningSpeed);
}
