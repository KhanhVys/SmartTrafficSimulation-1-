#include "core/Vehicle.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "core/CoreConstants.h"
#include "core/CruisingState.h"
#include "core/WorldView.h"

namespace {
constexpr double PI = 3.14159265358979323846;
}

Vehicle::Vehicle(int vehicleId, Direction direction, int lane, Movement movement, Route route,
                 double startFrontPosition, double maxSpeed)
    : vehicleId(vehicleId),
      direction(direction),
      lane(lane),
      movement(movement),
      route(std::move(route)),
      frontPosition(startFrontPosition),
      speed(maxSpeed),
      maxSpeed(maxSpeed),
      state(std::make_unique<CruisingState>()) {}

void Vehicle::update(double dt, const WorldView& view) {
    if (finished) {
        return;
    }
    std::unique_ptr<IVehicleState> next = state->update(*this, dt, view);
    if (next) {
        state = std::move(next);
    }
    // Xe coi như rời bản đồ khi đuôi xe vượt điểm cuối lộ trình.
    if (frontPosition - CAR_LENGTH >= route.path.getLength()) {
        finished = true;
        speed = 0.0;
    }
}

int Vehicle::getId() const { return vehicleId; }
Direction Vehicle::getDirection() const { return direction; }
int Vehicle::getLane() const { return lane; }
Movement Vehicle::getMovement() const { return movement; }
const Route& Vehicle::getRoute() const { return route; }
double Vehicle::getFrontPosition() const { return frontPosition; }
double Vehicle::getSpeed() const { return speed; }
double Vehicle::getMaxSpeed() const { return maxSpeed; }
const char* Vehicle::getStateName() const { return state->getName(); }
bool Vehicle::isFinished() const { return finished; }

void Vehicle::setMaxSpeed(double newMaxSpeed) { maxSpeed = std::max(0.0, newMaxSpeed); }

Vec2 Vehicle::getPosition() const {
    return route.path.getPositionAt(frontPosition - CAR_LENGTH / 2.0);
}

double Vehicle::getHeadingDegrees() const {
    const Vec2 dir = route.path.getDirectionAt(frontPosition - CAR_LENGTH / 2.0);
    return std::atan2(dir.y, dir.x) * 180.0 / PI;
}

bool Vehicle::isSignalingLeft() const {
    const bool leftGroup = (movement == Movement::Left || movement == Movement::UTurn);
    return leftGroup && (frontPosition - CAR_LENGTH / 2.0) < route.intersectionEndS;
}

bool Vehicle::isInTurn() const {
    return movement != Movement::Straight && frontPosition >= route.intersectionStartS &&
           (frontPosition - CAR_LENGTH) < route.intersectionEndS;
}

double Vehicle::getTargetSpeed() const {
    if (movement == Movement::Straight) {
        return maxSpeed;
    }
    // Giảm tốc từ trước vạch dừng cho tới khi đuôi xe ra khỏi đoạn cong.
    const bool inTurnZone = frontPosition >= route.stopLineS - TURN_SLOWDOWN_DISTANCE &&
                            (frontPosition - CAR_LENGTH) < route.intersectionEndS;
    return inTurnZone ? maxSpeed * TURN_SPEED_RATIO : maxSpeed;
}

double Vehicle::getBrakingDistance() const {
    return speed * speed / (2.0 * COMFORT_DECELERATION);
}

bool Vehicle::canStopBefore(double distance) const {
    return speed * speed <= 2.0 * HARD_DECELERATION * std::max(distance, 0.0);
}

bool Vehicle::getObstacleDistance(const WorldView& view, bool committedToStop, double& distance) const {
    double nearest = std::numeric_limits<double>::max();
    bool found = false;

    // Đèn chỉ có tác dụng với xe CHƯA qua vạch dừng. Qua vạch rồi thì đi tiếp.
    if (frontPosition <= route.stopLineS + LINE_CROSS_TOLERANCE) {
        const double toLine = std::max(route.stopLineS - frontPosition, 0.0);
        const LightColor color = view.getLightColor(direction, movement);
        bool mustStop = (color == LightColor::Red);
        if (color == LightColor::Yellow) {
            // Vàng: dừng nếu còn kịp phanh; không kịp thì đi tiếp cho an toàn.
            mustStop = committedToStop || canStopBefore(toLine);
        }
        if (mustStop) {
            nearest = toLine;
            found = true;
        }
    }

    double gap = 0.0;
    if (view.getLeaderGap(*this, gap)) {
        nearest = std::min(nearest, std::max(gap - MIN_FOLLOW_GAP, 0.0));
        found = true;
    }

    distance = nearest;
    return found;
}

void Vehicle::changeSpeed(double targetSpeed, double dt) {
    targetSpeed = std::clamp(targetSpeed, 0.0, maxSpeed);
    double newSpeed = speed;
    if (speed < targetSpeed) {
        newSpeed = std::min(targetSpeed, speed + ACCELERATION * dt);
    } else if (speed > targetSpeed) {
        newSpeed = std::max(targetSpeed, speed - COMFORT_DECELERATION * dt);
    }
    frontPosition += (speed + newSpeed) / 2.0 * dt;
    speed = newSpeed;
}

bool Vehicle::brakeToStop(double distance, double dt) {
    distance = std::max(distance, 0.0);
    // Gia tốc phanh vừa đủ để dừng đúng điểm đích, tối đa HARD_DECELERATION.
    const double required = (distance > 1e-6) ? speed * speed / (2.0 * distance) : HARD_DECELERATION;
    const double deceleration = std::min(required, HARD_DECELERATION);
    const double newSpeed = std::max(0.0, speed - deceleration * dt);
    const double travel = (speed + newSpeed) / 2.0 * dt;
    if (newSpeed <= STOP_SPEED_EPSILON || travel >= distance) {
        frontPosition += std::min(travel, distance);  // không bao giờ vượt điểm dừng
        speed = 0.0;
        return true;
    }
    frontPosition += travel;
    speed = newSpeed;
    return false;
}

void Vehicle::halt() { speed = 0.0; }
