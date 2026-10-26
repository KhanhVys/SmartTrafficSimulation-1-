#include "Vehicle.h"
#include "States.h"
#include "common/Constants.h"
#include <cmath>
#include <limits>

Vehicle::Vehicle(int id, Direction dir, Movement mov, float sx, float sy, int laneIdx)
    : id(id), laneIndex(laneIdx), direction(dir), heading(dir), movement(mov), x(sx), y(sy),
      speed(0.0f), maxSpeed(DEFAULT_MAX_SPEED), acceleration(DEFAULT_ACCELERATION), 
      deceleration(DEFAULT_DECELERATION), distanceToStopLine(9999.0f), 
      vehicleAhead(nullptr), safeDistance(DEFAULT_SAFE_DISTANCE),
      brakeForLight(false), brakeForVehicle(false), canProceedFlag(false), 
      shouldTurnFlag(false), hasTurned(false), ignoreLight(false), currentLightColor(LightColor::Red),
      turnProgress(0.0f), turnPhase(0), p0x(0), p0y(0), p1x(0), p1y(0), p2x(0), p2y(0)
{
    currentState = std::make_unique<AcceleratingState>();
}

Vehicle::~Vehicle() = default;

bool Vehicle::canStopBeforeStopLine() const {
    if (distanceToStopLine <= 0.0f) return true;
    float stoppingDist = (speed * speed) / (2.0f * deceleration);
    return stoppingDist <= distanceToStopLine;
}

bool Vehicle::shouldBrake() const {
    if (ignoreLight) return brakeForVehicle;
    return brakeForLight || brakeForVehicle;
}

bool Vehicle::isOutOfBounds() const {
    return (x < MAP_MIN_X || x > MAP_MAX_X || y < MAP_MIN_Y || y > MAP_MAX_Y);
}

void Vehicle::checkVehicleAhead(const std::vector<std::unique_ptr<Vehicle>>& allVehicles) {
    vehicleAhead = nullptr;
    float minDist = std::numeric_limits<float>::max();
    
    for (const auto& other : allVehicles) {
        if (other.get() == this) continue;
        if (other->getDirection() != direction) continue;
        if (other->getLaneIndex() != laneIndex) continue; 

        float dx = other->x - x;
        float dy = other->y - y;
        float d = std::sqrt(dx*dx + dy*dy);
        
        bool ahead = false;
        switch (direction) {
            case Direction::South: ahead = other->y > y; break;
            case Direction::North: ahead = other->y < y; break;
            case Direction::East:  ahead = other->x > x; break;
            case Direction::West:  ahead = other->x < x; break;
        }
        
        if (ahead && d < minDist) {
            minDist = d;
            vehicleAhead = other.get();
        }
    }
}

bool Vehicle::isSafeToMove() const {
    if (vehicleAhead == nullptr) return true;
    float dx = vehicleAhead->x - x;
    float dy = vehicleAhead->y - y;
    return std::sqrt(dx*dx + dy*dy) > safeDistance;
}

void Vehicle::update(float dt) {
    if (brakeForLight && currentLightColor == LightColor::Yellow && !canStopBeforeStopLine() && !hasPassedStopLine()) {
        ignoreLight = true;
    } else if (hasPassedStopLine() || currentLightColor == LightColor::Red) {
        ignoreLight = false; 
    }

    brakeForVehicle = !isSafeToMove();
    if (currentState) currentState->update(*this, dt);
}

void Vehicle::setState(std::unique_ptr<IVehicleState> s) { currentState = std::move(s); }

void Vehicle::moveForward(float dt) {
    if (!isSafeToMove()) return;
    float d = speed * dt;

    if (shouldBrake() && distanceToStopLine > 0.0f) {
        if (d >= distanceToStopLine) {
            d = distanceToStopLine;
            speed = 0; 
        }
    }

    switch (heading) { 
        case Direction::North: y -= d; break;
        case Direction::South: y += d; break;
        case Direction::East:  x += d; break;
        case Direction::West:  x -= d; break;
    }
    distanceToStopLine -= d;
}

void Vehicle::startTurning() {
    shouldTurnFlag = false; 
    hasTurned = true; 
    p0x = x; p0y = y;
    turnProgress = 0.0f;
    turnPhase = 0;
    float R = TURN_RADIUS;
    
    if (heading == Direction::South) {
        if (movement == Movement::Right) { p2x = x - R; p2y = y + R; p1x = x - R; p1y = y; heading = Direction::West; }
        else if (movement == Movement::Left) { p2x = x + R; p2y = y + R; p1x = x + R; p1y = y; heading = Direction::East; }
        else if (movement == Movement::UTurn) { p2x = x; p2y = y + 2*R; p1x = x - R; p1y = y + R; heading = Direction::North; } 
    } else if (heading == Direction::North) {
        if (movement == Movement::Right) { p2x = x + R; p2y = y - R; p1x = x + R; p1y = y; heading = Direction::East; }
        else if (movement == Movement::Left) { p2x = x - R; p2y = y - R; p1x = x - R; p1y = y; heading = Direction::West; }
        else if (movement == Movement::UTurn) { p2x = x; p2y = y - 2*R; p1x = x + R; p1y = y - R; heading = Direction::South; }
    } else if (heading == Direction::East) {
        if (movement == Movement::Right) { p2x = x + R; p2y = y + R; p1x = x; p1y = y + R; heading = Direction::South; }
        else if (movement == Movement::Left) { p2x = x + R; p2y = y - R; p1x = x; p1y = y - R; heading = Direction::North; }
        else if (movement == Movement::UTurn) { p2x = x + 2*R; p2y = y; p1x = x + R; p1y = y - R; heading = Direction::West; }
    } else if (heading == Direction::West) {
        if (movement == Movement::Right) { p2x = x - R; p2y = y - R; p1x = x; p1y = y - R; heading = Direction::North; }
        else if (movement == Movement::Left) { p2x = x - R; p2y = y + R; p1x = x; p1y = y + R; heading = Direction::South; }
        else if (movement == Movement::UTurn) { p2x = x - 2*R; p2y = y; p1x = x - R; p1y = y + R; heading = Direction::East; }
    }
}

void Vehicle::updateTurning(float dt) {
    float turnSpeed = (speed > 10.0f) ? (speed / TURN_RADIUS) : 1.5f; 
    turnProgress += dt * turnSpeed; 
    
    if (movement == Movement::UTurn && turnProgress >= 1.0f && turnPhase == 0) {
        turnPhase = 1;
        turnProgress = 0.0f;
        p0x = p2x; p0y = p2y;
        if (heading == Direction::North) { p1x = p0x + TURN_RADIUS; p1y = p0y - TURN_RADIUS; p2x = p0x; p2y = p0y - 2*TURN_RADIUS; }
        else if (heading == Direction::South) { p1x = p0x - TURN_RADIUS; p1y = p0y + TURN_RADIUS; p2x = p0x; p2y = p0y + 2*TURN_RADIUS; }
        else if (heading == Direction::East) { p1x = p0x + TURN_RADIUS; p1y = p0y + TURN_RADIUS; p2x = p0x + 2*TURN_RADIUS; p2y = p0y; }
        else if (heading == Direction::West) { p1x = p0x - TURN_RADIUS; p1y = p0y - TURN_RADIUS; p2x = p0x - 2*TURN_RADIUS; p2y = p0y; }
        return;
    }

    if (turnProgress >= 1.0f) {
        turnProgress = 1.0f;
        x = p2x; y = p2y;
        return;
    }
    
    float t = turnProgress;
    float u = 1.0f - t;
    x = u * u * p0x + 2 * u * t * p1x + t * t * p2x;
    y = u * u * p0y + 2 * u * t * p1y + t * t * p2y;
}

bool Vehicle::isTurnComplete() const { 
    if (movement == Movement::UTurn && turnPhase == 1 && turnProgress >= 1.0f) return true;
    if (movement != Movement::UTurn && turnProgress >= 1.0f) return true;
    return false; 
}