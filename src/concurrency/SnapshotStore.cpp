#include "SnapshotStore.h"

void SnapshotStore::publishSnapshot(
    std::size_t tick,
    const std::vector<LightSnapshot>& newLights,
    const std::vector<VehicleSnapshot>& newVehicles) {
    // Thứ tự khóa bắt buộc: lightMutex trước, vehicleMutex sau.
    // getSnapshot() dùng đúng thứ tự này để tránh deadlock.
    std::lock_guard<std::mutex> lightLock(lightMutex);
    std::lock_guard<std::mutex> vehicleLock(vehicleMutex);

    lights = newLights;
    vehicles = newVehicles;
    currentTick.store(tick);
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
