#pragma once
#include <vector>
#include <memory>
#include "Vehicle.h"
#include "IRoadComponent.h"

class Road : public IRoadComponent {
private:
    Direction direction;
    int laneCount;
    std::vector<std::shared_ptr<Vehicle>> vehicleList;

public:
    Road(Direction dir, int lanes);

    void addVehicle(std::shared_ptr<Vehicle> vehicle);
    void updateVehicles(double dt, bool isRedLightForLane, int laneIndex);
    void displayInfo() const override;

    const std::vector<std::shared_ptr<Vehicle>>& getVehicleList() const { return vehicleList; }
    Direction getDirection() const { return direction; }
    int getLaneCount() const { return laneCount; }
};
