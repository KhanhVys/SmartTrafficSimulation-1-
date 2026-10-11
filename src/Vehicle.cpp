#include "Vehicle.h"

Vehicle::Vehicle(std::string id, double startPos, double startSpeed)
    : vehicleId(id), position(startPos), speed(startSpeed),
      maxSpeed(13.89), length(4.5), safeDistance(10.0),
      acceleration(2.5), deceleration(4.5) {
    currentState = std::make_shared<CruisingState>();
}

void Vehicle::setState(std::shared_ptr<IVehicleState> newState) {
    currentState = newState;
}

void Vehicle::move(double dt) {
    position += speed * dt;
}

void Vehicle::update(double distanceToNext, bool isRedLight, double dt) {
    if (currentState) {
        currentState->handle(*this, distanceToNext, isRedLight, dt);
    }
}