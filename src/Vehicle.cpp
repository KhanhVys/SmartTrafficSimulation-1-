#include "Vehicle.h"
#include "States.h"
#include "common/Constants.h"
#include <cmath>

Vehicle::Vehicle(int id, Direction dir, Movement mov, float sx, float sy, int laneIdx)
    : id(id), laneIndex(laneIdx), direction(dir), heading(dir), movement(mov), x(sx), y(sy),
      speed(0.0f), maxSpeed(DEFAULT_MAX_SPEED), acceleration(DEFAULT_ACCELERATION), 
      deceleration(DEFAULT_DECELERATION), distanceToStopLine(9999.0f), 
      vehicleAhead(nullptr), safeDistance(DEFAULT_SAFE_DISTANCE),
      brakeForLight(false), brakeForVehicle(false), canProceedFlag(false), 
      shouldTurnFlag(false), ignoreLight(false), currentLightColor(LightColor::Red),
      turnProgress(0.0f), p0x(0), p0y(0), p1x(0), p1y(0), p2x(0), p2y(0)
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

bool Vehicle::isSafeToMove() const {
    if (vehicleAhead == nullptr) return true;
    float dx = vehicleAhead->x - x;
    float dy = vehicleAhead->y - y;
    return std::sqrt(dx*dx + dy*dy) > safeDistance;
}

void Vehicle::update(float dt) {
    // Fix vấn đề 3: CHỈ bỏ qua đèn khi đèn VÀNG. Đèn ĐỎ thì phải dừng.
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
        if (d >= distanceToStopLine) d = distanceToStopLine; 
    }

    switch (heading) { // Dùng heading thay vì direction
        case Direction::North: y -= d; break;
        case Direction::South: y += d; break;
        case Direction::East:  x += d; break;
        case Direction::West:  x -= d; break;
    }
    distanceToStopLine -= d;
}

void Vehicle::startTurning() {
    // Fix vấn đề 1: Tắt cờ rẽ ngay lập tức
    shouldTurnFlag = false; 
    
    p0x = x; p0y = y;
    turnProgress = 0.0f;
    float R = TURN_RADIUS;
    
    // Fix vấn đề 4 & 8: Dùng Bezier để vẽ đường cong (P0, P1, P2)
    if (heading == Direction::South) {
        if (movement == Movement::Right) { p2x = x - R; p2y = y + R; p1x = x - R; p1y = y; heading = Direction::West; }
        else if (movement == Movement::Left) { p2x = x + R; p2y = y + R; p1x = x + R; p1y = y; heading = Direction::East; }
        else if (movement == Movement::UTurn) { p2x = x; p2y = y + 2*R; p1x = x - R; p1y = y + R; heading = Direction::North; } // UTurn thì dùng 2 cung
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
    turnProgress += dt * 2.0f; // Tốc độ rẽ
    
    if (turnProgress >= 1.0f) {
        turnProgress = 1.0f;
        x = p2x; y = p2y;
        return;
    }
    
    // Công thức đường cong Bezier bậc 2
    float t = turnProgress;
    float u = 1.0f - t;
    x = u * u * p0x + 2 * u * t * p1x + t * t * p2x;
    y = u * u * p0y + 2 * u * t * p1y + t * t * p2y;
}

bool Vehicle::isTurnComplete() const { return turnProgress >= 1.0f; }