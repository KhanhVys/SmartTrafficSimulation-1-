#pragma once
#include <string>
#include <memory>
#include "common/Types.h"
#include "IVehicleState.h"

class Vehicle {
private:
    int vehicleId; 
    Direction direction;
    Movement movement;
    int laneIndex;
    
    double speed;
    double maxSpeed;
    double acceleration;
    double deceleration;
    double position; 
    double x;
    double y;
    
    std::unique_ptr<IVehicleState> currentState;

public:
    Vehicle(int id, Direction dir, Movement move, int lane, double maxSpd = 35.0, double startPos = 150.0);

    void update(double dt, double distanceToStopLine, bool isRedLight, bool clearAhead);
    void setState(std::unique_ptr<IVehicleState> newState);

    int getVehicleId() const { return vehicleId; }
    Direction getDirection() const { return direction; }
    Movement getMovement() const { return movement; }
    int getLaneIndex() const { return laneIndex; }
    double getSpeed() const { return speed; }
    void setSpeed(double spd) { speed = spd; }
    double getMaxSpeed() const { return maxSpeed; }
    double getAcceleration() const { return acceleration; }
    double getDeceleration() const { return deceleration; }
    double getPosition() const { return position; }
    double getX() const { return x; }
    double getY() const { return y; }
    std::string getStateName() const { return currentState ? currentState->getStateName() : "Unknown"; }
};
