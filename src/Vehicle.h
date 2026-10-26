#pragma once
#include "common/Types.h"
#include <memory>
#include <vector>

class IVehicleState;

class Vehicle {
public:
    Vehicle(int id, Direction direction, Movement movement, float startX, float startY, int laneIndex);
    ~Vehicle();

    void update(float dt);
    void setState(std::unique_ptr<IVehicleState> newState);
    IVehicleState* getState() const { return currentState.get(); }

    int       getId() const { return id; }
    int       getLaneIndex() const { return laneIndex; }
    Point     getPosition() const { return {x, y}; }
    Direction getDirection() const { return direction; }
    Movement  getMovement() const { return movement; }
    float     getSpeed() const { return speed; }
    float     getMaxSpeed() const { return maxSpeed; }
    float     getAcceleration() const { return acceleration; }
    float     getDeceleration() const { return deceleration; }
    void      setSpeed(float s) { speed = s; }

    void  setDistanceToStopLine(float d) { distanceToStopLine = d; }
    float getDistanceToStopLine() const { return distanceToStopLine; }
    bool  hasPassedStopLine() const { return distanceToStopLine < 0.0f; }
    bool  canStopBeforeStopLine() const;

    void  checkVehicleAhead(const std::vector<std::unique_ptr<Vehicle>>& allVehicles);
    bool  isSafeToMove() const;
    bool  isOutOfBounds() const;

    void setBrakeForLight(bool b)   { brakeForLight = b; }
    void setCanProceed(bool b) { canProceedFlag = b; }
    bool canProceed() const    { return canProceedFlag; }
    bool shouldBrake() const; 
    
    void setLightColor(LightColor c) { currentLightColor = c; }
    LightColor getLightColor() const { return currentLightColor; }

    void setShouldTurn(bool b) { shouldTurnFlag = b; }
    bool shouldTurn() const    { return shouldTurnFlag; }
    bool getHasTurned() const  { return hasTurned; }
    void setHasTurned(bool b)  { hasTurned = b; }

    void moveForward(float dt);
    void startTurning();
    void updateTurning(float dt);
    bool isTurnComplete() const;

private:
    int       id;
    int       laneIndex;
    Direction direction;     
    Direction heading;       
    Movement  movement;
    float     x, y;
    float     speed, maxSpeed, acceleration, deceleration;

    float    distanceToStopLine;
    Vehicle* vehicleAhead;
    float    safeDistance;

    bool brakeForLight;
    bool brakeForVehicle;
    bool canProceedFlag;
    bool shouldTurnFlag;
    bool hasTurned;
    bool ignoreLight;
    LightColor currentLightColor;

    std::unique_ptr<IVehicleState> currentState;
    
    float turnProgress;
    int turnPhase;
    float p0x, p0y, p1x, p1y, p2x, p2y; 
};