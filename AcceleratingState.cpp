#include "core/AcceleratingState.h"

#include <cmath>

#include "core/BrakingState.h"
#include "core/CoreConstants.h"
#include "core/CruisingState.h"
#include "core/TurningState.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"

const char* AcceleratingState::getName() const { return "AcceleratingState"; }

std::unique_ptr<IVehicleState> AcceleratingState::update(Vehicle& vehicle, double dt, const WorldView& view) {
    if (vehicle.isInTurn()) {
        return std::make_unique<TurningState>();
    }
    double distance = 0.0;
    if (vehicle.getObstacleDistance(view, false, distance) && vehicle.getBrakingDistance() >= distance) {
        return std::make_unique<BrakingState>();
    }
    const double target = vehicle.getTargetSpeed();
    vehicle.changeSpeed(target, dt);
    if (std::abs(vehicle.getSpeed() - target) <= SPEED_EPSILON) {
        return std::make_unique<CruisingState>();
    }
    return nullptr;
}
