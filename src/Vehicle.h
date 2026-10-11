#pragma once
#include <string>
#include <memory>
#include "Enums.h"
#include "IVehicleState.h"

class Vehicle {
private:
    std::string id;
    Direction direction;
    TurnType turnType;
    int laneIndex;
    
    double speed;
    double maxSpeed;
    double acceleration;
    double deceleration;
    double position;
    
    std::unique_ptr<IVehicleState> currentState;

public:
    Vehicle(std::string id, Direction dir, TurnType turn, int lane, double maxSpd = 35.0);

    void update(double dt, double distanceToStopLine, bool isRedLight, bool clearAhead);
    void setState(std::unique_ptr<IVehicleState> newState);

    std::string getId() const { return id; }
    Direction getDirection() const { return direction; }
    TurnType getTurnType() const { return turnType; }
    int getLaneIndex() const { return laneIndex; }
    double getSpeed() const { return speed; }
    void setSpeed(double spd) { speed = spd; }
    double getMaxSpeed() const { return maxSpeed; }
    double getAcceleration() const { return acceleration; }
    double getDeceleration() const { return deceleration; }
    double getPosition() const { return position; }
    std::string getStateName() const { return currentState ? currentState->getStateName() : "Unknown"; }
};