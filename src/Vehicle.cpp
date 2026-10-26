#include "Vehicle.h"
#include "IVehicleState.h"
#include "AcceleratingState.h"
#include <cmath>

Vehicle::Vehicle(int id, Direction dir, Movement mov, float sx, float sy)
    : id(id), direction(dir), movement(mov), x(sx), y(sy),
      speed(0.0f), maxSpeed(Config::DEFAULT_MAX_SPEED),
      acceleration(Config::DEFAULT_ACCELERATION), braking(Config::DEFAULT_BRAKING),
      distanceToStopLine(9999.0f), vehicleAhead(nullptr), safeDistance(Config::DEFAULT_SAFE_DISTANCE),
      brakeForLight(false), brakeForVehicle(false), canProceedFlag(false), shouldTurnFlag(false), turnProgress(0.0f)
{
    currentState = std::make_unique<AcceleratingState>();
}

Vehicle::~Vehicle() = default;

void Vehicle::setDistanceToStopLine(float d) { distanceToStopLine = (d > 0.0f) ? d : 0.0f; }

bool Vehicle::canStopBeforeStopLine() const {
    if (distanceToStopLine <= 0.0f) return true;
    float stoppingDist = (speed * speed) / (2.0f * braking);
    return stoppingDist <= distanceToStopLine;
}

bool Vehicle::isSafeToMove() const {
    if (vehicleAhead == nullptr) return true;
    float dx = vehicleAhead->x - x;
    float dy = vehicleAhead->y - y;
    return std::sqrt(dx*dx + dy*dy) > safeDistance;
}

void Vehicle::update(float dt) {
    brakeForVehicle = !isSafeToMove();
    if (currentState) currentState->update(*this, dt);
}

void Vehicle::setState(std::unique_ptr<IVehicleState> s) { currentState = std::move(s); }

void Vehicle::moveForward(float dt) {
    if (!isSafeToMove()) return;
    float d = speed * dt;

    if (shouldBrake() && distanceToStopLine > 0.0f) {
        if (d >= distanceToStopLine) {
            d = distanceToStopLine - 0.01f;
            if (d < 0.0f) d = 0.0f;
        }
    }

    switch (direction) {
        case Direction::North: y -= d; break;
        case Direction::South: y += d; break;
        case Direction::East:  x += d; break;
        case Direction::West:  x -= d; break;
    }

    if (distanceToStopLine > 0.0f) {
        distanceToStopLine -= d;
        if (distanceToStopLine < 0.0f) distanceToStopLine = 0.0f;
    }
}

void Vehicle::updateTurning(float dt) {
    turnProgress += dt;
    if (turnProgress > 1.0f) turnProgress = 1.0f;
}

bool Vehicle::isTurnComplete() const { return turnProgress >= 1.0f; }