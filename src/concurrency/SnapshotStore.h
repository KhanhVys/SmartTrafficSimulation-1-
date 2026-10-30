#pragma once

#include "SimulationSnapshot.h"

#include <atomic>
#include <mutex>
#include <vector>

class SnapshotStore {
public:
    void publishSnapshot(
        std::size_t tick,
        const std::vector<LightSnapshot>& lights,
        const std::vector<VehicleSnapshot>& vehicles
    );

    SimulationSnapshot getSnapshot() const;

private:
    mutable std::mutex vehicleMutex;
    mutable std::mutex lightMutex;

    std::atomic<std::size_t> currentTick{0};
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
