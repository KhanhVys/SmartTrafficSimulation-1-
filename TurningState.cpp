#include "core/TurningState.h"

#include "core/AcceleratingState.h"
#include "core/BrakingState.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"

const char* TurningState::getName() const { return "TurningState"; }

// Đang đi qua đoạn cong của ngã tư (rẽ trái, rẽ phải, quay đầu) với tốc độ rẽ.
// Xe đã qua vạch dừng nên không còn xét đèn; chỉ xét xe phía trước.
std::unique_ptr<IVehicleState> TurningState::update(Vehicle& vehicle, double dt, const WorldView& view) {
    double distance = 0.0;
    if (vehicle.getObstacleDistance(view, false, distance) && vehicle.getBrakingDistance() >= distance) {
        return std::make_unique<BrakingState>();
    }
    if (!vehicle.isInTurn()) {
        return std::make_unique<AcceleratingState>();
    }
    vehicle.changeSpeed(vehicle.getTargetSpeed(), dt);
    return nullptr;
}
