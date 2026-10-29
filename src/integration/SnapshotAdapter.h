#pragma once

#include <vector>

#include "concurrency/SimulationSnapshot.h"
#include "core/CoreSimulation.h"

namespace SnapshotAdapter {

inline std::vector<VehicleSnapshot> vehiclesOf(const CoreSimulation& sim) {
    std::vector<VehicleSnapshot> result;
    for (const auto& vehicle : sim.getVehicles()) {
        if (vehicle->isFinished()) {
            continue;
        }
        const Vec2 position = vehicle->getPosition();
        VehicleSnapshot snapshot;
        snapshot.vehicleId = vehicle->getId();
        snapshot.x = position.x;
        snapshot.y = position.y;
        snapshot.direction = vehicle->getDirection();
        snapshot.movement = vehicle->getMovement();
        snapshot.rotationAngleDegrees = vehicle->getHeadingDegrees();
        snapshot.signalingLeft = vehicle->isSignalingLeft();
        result.push_back(snapshot);
    }
    return result;
}

inline std::vector<LightSnapshot> lightsOf(const CoreSimulation& sim) {
    const Direction arms[] = {Direction::North, Direction::South, Direction::East, Direction::West};
    std::vector<LightSnapshot> result;
    for (Direction arm : arms) {
        for (bool turnArrow : {false, true}) {
            LightSnapshot light;
            light.direction = arm;
            light.turnArrow = turnArrow;
            light.color = sim.getLightColor(arm, turnArrow ? Movement::Left : Movement::Straight);
            result.push_back(light);
        }
    }
    return result;
}

}  // namespace SnapshotAdapter
