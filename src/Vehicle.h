#pragma once
#include "common/Types.h"
#include <memory>

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
    Direction getDirection() const { return direction; }
    Movement  getMovement() const { return movement; }
    Point     getPosition() const { return {x, y}; }
    float     getSpeed() const { return speed; }
    float     getMaxSpeed() const { return maxSpeed; }
    float     getAcceleration() const { return acceleration; }
    float     getDeceleration() const { return deceleration; }
    void      setSpeed(float s) { speed = s; }

    void  setDistanceToStopLine(float d) { distanceToStopLine = d; }
    float getDistanceToStopLine() const { return distanceToStopLine; }
    bool  hasPassedStopLine() const { return distanceToStopLine < 0.0f; }
    bool  canStopBeforeStopLine() const;

    void setVehicleAhead(Vehicle* v) { vehicleAhead = v; }
    Vehicle* getVehicleAhead() const { return vehicleAhead; }
    bool  isSafeToMove() const;
    bool  isOutOfBounds() const;

    void setBrakeForLight(bool b)   { brakeForLight = b; }
    void setCanProceed(bool b) { canProceedFlag = b; }
    bool canProceed() const    { return canProceedFlag; }
    bool shouldBrake() const;
    void setIgnoreLight(bool ignore) { ignoreLight = ignore; }

    void setShouldTurn(bool b) { shouldTurnFlag = b; }
    bool shouldTurn() const    { return shouldTurnFlag; }

    void moveForward(float dt);
    void startTurning();
    void updateTurning(float dt);
    bool isTurnComplete() const;

private:
    int       id;
    int       laneIndex;
    Direction direction;
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
    bool ignoreLight; // Cờ bỏ qua đèn khi không kịp dừng

    std::unique_ptr<IVehicleState> currentState;

    float turnProgress;
    float turnStartX, turnStartY;
    float turnTargetX, turnTargetY;
};