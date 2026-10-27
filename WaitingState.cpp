#include "core/WaitingState.h"

#include "core/AcceleratingState.h"
#include "core/CoreConstants.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"

const char* WaitingState::getName() const { return "WaitingState"; }

// Đứng yên chờ đèn hoặc chờ xe trước. Hết vật cản thì tăng tốc.
std::unique_ptr<IVehicleState> WaitingState::update(Vehicle& vehicle, double, const WorldView& view) {
    double distance = 0.0;
    if (vehicle.getObstacleDistance(view, false, distance) && distance <= WAIT_TOLERANCE) {
        vehicle.halt();
        return nullptr;
    }
    return std::make_unique<AcceleratingState>();
}
