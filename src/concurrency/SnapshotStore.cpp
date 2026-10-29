#include "SnapshotStore.h"

void SnapshotStore::publishTick(std::size_t tick) {
    // currentTick chỉ được ghi bởi luồng mô phỏng trong prototype này.
    currentTick.store(tick);
}

void SnapshotStore::publishVehicles(
    const std::vector<VehicleSnapshot>& newVehicles) {
    std::lock_guard<std::mutex> lock(vehicleMutex);
    vehicles = newVehicles;
}

void SnapshotStore::publishLights(
    const std::vector<LightSnapshot>& newLights) {
    std::lock_guard<std::mutex> lock(lightMutex);
    lights = newLights;
}

SimulationSnapshot SnapshotStore::getSnapshot() const {
    SimulationSnapshot snapshot;

    // Nếu phải lấy cả hai loại dữ liệu, luôn khóa đèn trước rồi đến xe.
    std::lock_guard<std::mutex> lightLock(lightMutex);
    std::lock_guard<std::mutex> vehicleLock(vehicleMutex);

    snapshot.tick = currentTick.load();
    snapshot.lights = lights;
    snapshot.vehicles = vehicles;

    // Trả bản sao; GUI không giữ mutex trong lúc render.
    return snapshot;
}
