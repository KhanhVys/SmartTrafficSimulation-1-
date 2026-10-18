#pragma once
#include <string>
#include <memory>
#include "Types.h"
#include "IVehicleState.h"

class Vehicle {
private:
    std::string vehicleId;
    Direction direction;
    Movement movement;
    int laneIndex;
    
    double speed;
    double maxSpeed;
    double acceleration;
    double deceleration;
    double position; 
    
    std::unique_ptr<IVehicleState> currentState;

public:
    Vehicle(std::string id, Direction dir, Movement move, int lane, double maxSpd = 35.0, double startPos = 150.0);

    void update(double dt, double distanceToStopLine, bool isRedLight, bool clearAhead);
    void setState(std::unique_ptr<IVehicleState> newState);

    std::string getVehicleId() const { return vehicleId; }
    Direction getDirection() const { return direction; }
    Movement getMovement() const { return movement; }
    int getLaneIndex() const { return laneIndex; }
    double getSpeed() const { return speed; }
    void setSpeed(double spd) { speed = spd; }
    double getMaxSpeed() const { return maxSpeed; }
    double getAcceleration() const { return acceleration; }
    double getDeceleration() const { return deceleration; }
    double getPosition() const { return position; }
    std::string getStateName() const { return currentState ? currentState->getStateName() : "Unknown"; }
};