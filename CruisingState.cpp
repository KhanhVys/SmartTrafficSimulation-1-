#include "core/CruisingState.h"

#include <cmath>

#include "core/AcceleratingState.h"
#include "core/BrakingState.h"
#include "core/CoreConstants.h"
#include "core/TurningState.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"

const char* CruisingState::getName() const { return "CruisingState"; }

std::unique_ptr<IVehicleState> CruisingState::update(Vehicle& vehicle, double dt, const WorldView& view) {
    if (vehicle.isInTurn()) {
        return std::make_unique<TurningState>();
    }
    double distance = 0.0;
    if (vehicle.getObstacleDistance(view, false, distance) && vehicle.getBrakingDistance() >= distance) {
        return std::make_unique<BrakingState>();
    }
    const double target = vehicle.getTargetSpeed();
    if (vehicle.getSpeed() > target + SPEED_EPSILON) {
        return std::make_unique<BrakingState>();  // giảm tốc trước khúc cua
    }
    if (vehicle.getSpeed() < target - SPEED_EPSILON) {
        return std::make_unique<AcceleratingState>();
    }
    vehicle.changeSpeed(target, dt);
    return nullptr;
}
