#include "Vehicle.h"
#include "States.h"
#include "common/Constants.h"
#include <cmath>

Vehicle::Vehicle(int id, Direction dir, Movement mov, float sx, float sy, int laneIdx)
    : id(id), laneIndex(laneIdx), direction(dir), movement(mov), x(sx), y(sy),
      speed(0.0f), maxSpeed(DEFAULT_MAX_SPEED), acceleration(DEFAULT_ACCELERATION), 
      deceleration(DEFAULT_DECELERATION), distanceToStopLine(9999.0f), 
      vehicleAhead(nullptr), safeDistance(DEFAULT_SAFE_DISTANCE),
      brakeForLight(false), brakeForVehicle(false), canProceedFlag(false), 
      shouldTurnFlag(false), ignoreLight(false),
      turnProgress(0.0f), turnStartX(0), turnStartY(0), turnTargetX(0), turnTargetY(0)
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
    return (x < -200.0f || x > 1000.0f || y < -200.0f || y > 800.0f);
}

bool Vehicle::isSafeToMove() const {
    if (vehicleAhead == nullptr) return true;
    float dx = vehicleAhead->x - x;
    float dy = vehicleAhead->y - y;
    return std::sqrt(dx*dx + dy*dy) > safeDistance;
}

void Vehicle::update(float dt) {
    // Kiểm tra nếu không kịp dừng đèn vàng thì bỏ qua đèn
    if (brakeForLight && !canStopBeforeStopLine() && !hasPassedStopLine()) {
        ignoreLight = true;
    } else if (hasPassedStopLine()) {
        ignoreLight = false; // Đã qua vạch thì reset
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
            d = distanceToStopLine; // Dừng đúng vạch
        }
    }

    switch (direction) {
        case Direction::North: y -= d; break;
        case Direction::South: y += d; break;
        case Direction::East:  x += d; break;
        case Direction::West:  x -= d; break;
    }
    distanceToStopLine -= d;
}

void Vehicle::startTurning() {
    turnStartX = x;
    turnStartY = y;
    turnProgress = 0.0f;
    float R = 60.0f; // Bán kính rẽ rộng hơn chút
    
    // Xác định tọa độ đích dựa trên hướng hiện tại và hướng rẽ
    if (direction == Direction::South) {
        if (movement == Movement::Right) { turnTargetX = x - R; turnTargetY = y + R; direction = Direction::West; }
        else if (movement == Movement::Left) { turnTargetX = x + R; turnTargetY = y + R; direction = Direction::East; }
        else if (movement == Movement::UTurn) { turnTargetX = x; turnTargetY = y + 2*R; direction = Direction::North; }
    } else if (direction == Direction::North) {
        if (movement == Movement::Right) { turnTargetX = x + R; turnTargetY = y - R; direction = Direction::East; }
        else if (movement == Movement::Left) { turnTargetX = x - R; turnTargetY = y - R; direction = Direction::West; }
        else if (movement == Movement::UTurn) { turnTargetX = x; turnTargetY = y - 2*R; direction = Direction::South; }
    } else if (direction == Direction::East) {
        if (movement == Movement::Right) { turnTargetX = x + R; turnTargetY = y + R; direction = Direction::South; }
        else if (movement == Movement::Left) { turnTargetX = x + R; turnTargetY = y - R; direction = Direction::North; }
        else if (movement == Movement::UTurn) { turnTargetX = x + 2*R; turnTargetY = y; direction = Direction::West; }
    } else if (direction == Direction::West) {
        if (movement == Movement::Right) { turnTargetX = x - R; turnTargetY = y - R; direction = Direction::North; }
        else if (movement == Movement::Left) { turnTargetX = x - R; turnTargetY = y + R; direction = Direction::South; }
        else if (movement == Movement::UTurn) { turnTargetX = x - 2*R; turnTargetY = y; direction = Direction::East; }
    }
}

void Vehicle::updateTurning(float dt) {
    // Tốc độ rẽ nhanh hơn để test (2.0f)
    turnProgress += dt * 2.0f; 
    
    if (turnProgress >= 1.0f) {
        turnProgress = 1.0f;
        x = turnTargetX;
        y = turnTargetY;
        return;
    }
    
    // Nội suy tuyến tính vị trí
    x = turnStartX + (turnTargetX - turnStartX) * turnProgress;
    y = turnStartY + (turnTargetY - turnStartY) * turnProgress;
}

bool Vehicle::isTurnComplete() const { return turnProgress >= 1.0f; }