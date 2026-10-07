#pragma once

#include "SimulationSnapshot.h"

#include <atomic>
#include <mutex>
#include <vector>

class SnapshotStore {
public:
    void publishTick(std::size_t tick);
    void publishVehicles(const std::vector<VehicleSnapshot>& vehicles);
    void publishLights(const std::vector<LightSnapshot>& lights);

    SimulationSnapshot getSnapshot() const;

private:
    mutable std::mutex vehicleMutex;
    mutable std::mutex lightMutex;

    std::atomic<std::size_t> currentTick{0};
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
