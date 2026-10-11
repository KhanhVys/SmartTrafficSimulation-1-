#pragma once
#include <vector>
#include <memory>
#include "Vehicle.h"
#include "Enums.h"

class Road {
private:
    Direction direction;
    int laneCount;
    std::vector<std::shared_ptr<Vehicle>> vehicles;

public:
    Road(Direction dir, int lanes);

    void addVehicle(std::shared_ptr<Vehicle> vehicle);
    void updateVehicles(double dt, bool isRedLightForLane, int laneIndex);

    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const { return vehicles; }
    Direction getDirection() const { return direction; }
    int getLaneCount() const { return laneCount; }
};