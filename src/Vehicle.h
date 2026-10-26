#pragma once
#include "Types.h"
#include <memory>

class IVehicleState;

class Vehicle {
public:
    Vehicle(int id, Direction direction, Movement movement, float startX, float startY);
    ~Vehicle();

    void update(float dt);
    void setState(std::unique_ptr<IVehicleState> newState);
    IVehicleState* getState() const { return currentState.get(); }

    int       getId() const { return id; }
    Direction getDirection() const { return direction; }
    Movement  getMovement()  const { return movement; }
    float     getX() const { return x; }
    float     getY() const { return y; }
    float     getSpeed() const { return speed; }
    float     getMaxSpeed() const { return maxSpeed; }
    float     getAcceleration() const { return acceleration; }
    float     getBraking() const { return braking; }
    void      setSpeed(float s) { speed = s; }

    void  setDistanceToStopLine(float d) { distanceToStopLine = d; }
    float getDistanceToStopLine() const { return distanceToStopLine; }
    
    // FIX THEO YÊU CẦU CỦA LEADER: < 0 mới tính là đã qua vạch
    bool  hasPassedStopLine() const { return distanceToStopLine < 0.0f; } 
    
    bool  canStopBeforeStopLine() const;

    void setVehicleAhead(Vehicle* v) { vehicleAhead = v; }
    Vehicle* getVehicleAhead() const { return vehicleAhead; }
    bool  isSafeToMove() const;

    void setBrakeForLight(bool b)   { brakeForLight = b; }
    bool getBrakeForLight() const   { return brakeForLight; }
    void setCanProceed(bool b) { canProceedFlag = b; }
    bool canProceed() const    { return canProceedFlag; }
    bool shouldBrake() const { return brakeForLight || brakeForVehicle; }

    void setShouldTurn(bool b) { shouldTurnFlag = b; }
    bool shouldTurn() const    { return shouldTurnFlag; }

    void moveForward(float dt);
    void updateTurning(float dt);
    bool isTurnComplete() const;

private:
    int       id;
    Direction direction;
    Movement  movement;
    float     x, y;
    float     speed, maxSpeed, acceleration, braking;

    float    distanceToStopLine; 
    Vehicle* vehicleAhead;
    float    safeDistance;

    bool brakeForLight;
    bool brakeForVehicle;
    bool canProceedFlag;
    bool shouldTurnFlag;

    std::unique_ptr<IVehicleState> currentState;
    float turnProgress;
};