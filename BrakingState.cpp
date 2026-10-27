#include "core/BrakingState.h"

#include "core/AcceleratingState.h"
#include "core/CoreConstants.h"
#include "core/CruisingState.h"
#include "core/TurningState.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"
#include "core/WaitingState.h"

const char* BrakingState::getName() const { return "BrakingState"; }

std::unique_ptr<IVehicleState> BrakingState::update(Vehicle& vehicle, double dt, const WorldView& view) {
    double distance = 0.0;
    // committedToStop = true: đã phanh rồi thì đèn vàng cũng phải dừng (không đổi ý giữa chừng).
    if (vehicle.getObstacleDistance(view, true, distance)) {
        // Vật cản đã xa hẳn (xe trước đi mất): nhả phanh, tăng tốc lại.
        if (vehicle.getSpeed() > STOP_SPEED_EPSILON &&
            vehicle.getBrakingDistance() < distance * BRAKE_RELEASE_RATIO) {
            return std::make_unique<AcceleratingState>();
        }
        if (vehicle.brakeToStop(distance, dt)) {
            return std::make_unique<WaitingState>();
        }
        return nullptr;
    }

    // Không có vật cản: đây là giảm tốc để vào cua.
    if (vehicle.isInTurn()) {
        return std::make_unique<TurningState>();
    }
    const double target = vehicle.getTargetSpeed();
    if (vehicle.getSpeed() <= target + SPEED_EPSILON) {
        if (vehicle.getSpeed() < target - SPEED_EPSILON) {
            return std::make_unique<AcceleratingState>();
        }
        return std::make_unique<CruisingState>();
    }
    vehicle.changeSpeed(target, dt);
    return nullptr;
}
