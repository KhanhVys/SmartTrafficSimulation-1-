#pragma once

#include <cstddef>
#include <vector>

enum class LightColor { Red, Yellow, Green };

enum class Direction { North, South, East, West };

enum class Movement { Straight, Left, Right, UTurn };

struct VehicleSnapshot {
    int vehicleId = 0;
    double x = 0.0;
    double y = 0.0;
    Direction direction = Direction::North;
    Movement movement = Movement::Straight;
};

struct LightSnapshot {
    Direction direction = Direction::North;
    LightColor color = LightColor::Red;
};

struct SimulationSnapshot {
    std::size_t tick = 0;
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
