#pragma once

#include "common/Types.h"

#include <cstddef>
#include <vector>

struct VehicleSnapshot {
    int vehicleId = 0;
    double x = 0.0;
    double y = 0.0;
    Direction direction = Direction::North;
    Movement movement = Movement::Straight;
    bool signalingLeft = false;
    // Góc xoay hiển thị của xe, đơn vị độ; xe quay đầu thường là 180 độ.
    double rotationAngleDegrees = 0.0;
};

struct LightSnapshot {
    Direction direction = Direction::North;
    bool turnArrow = false;
    LightColor color = LightColor::Red;
};

struct SimulationSnapshot {
    std::size_t tick = 0;
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
