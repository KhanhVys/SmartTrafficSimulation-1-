#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <memory>
#include "VehicleState.h"

class Vehicle {
private:
    std::string vehicleId;
    double position;
    double speed;
    double maxSpeed;
    double length;
    double safeDistance;
    double acceleration;
    double deceleration;

    std::shared_ptr<IVehicleState> currentState;

public:
    Vehicle(std::string id, double startPos = 0.0, double startSpeed = 13.89);

    void update(double distanceToNext, bool isRedLight, double dt);
    void move(double dt);
    void setState(std::shared_ptr<IVehicleState> newState);

    std::string getId() const { return vehicleId; }
    double getPosition() const { return position; }
    double getSpeed() const { return speed; }
    double getSpeedKmh() const { return speed * 3.6; }
    double getMaxSpeed() const { return maxSpeed; }
    double getLength() const { return length; }
    double getSafeDistance() const { return safeDistance; }
    double getAcceleration() const { return acceleration; }
    double getDeceleration() const { return deceleration; }

    void setSpeed(double s) { speed = s; }
    std::string getStateName() const { return currentState ? currentState->getStateName() : "Unknown"; }
};

#endif