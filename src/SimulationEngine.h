#pragma once
#include <vector>
#include <memory>
#include "Intersection.h"
#include "Vehicle.h"

class SimulationEngine {
private:
    Intersection intersection;
    std::vector<std::shared_ptr<Vehicle>> allVehicles;
    double currentTickTime;

public:
    SimulationEngine();
    void initializeScenario();
    void step(double dt);

    double getTime() const { return currentTickTime; }
    const std::vector<std::shared_ptr<Vehicle>>& getAllVehicles() const { return allVehicles; }
    Intersection& getIntersection() { return intersection; }
};