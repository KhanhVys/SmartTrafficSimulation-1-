#pragma once
#include <vector>
#include <memory>
#include "TrafficLight.h"
#include "Road.h"
#include "Vehicle.h"

class SimulationEngine {
private:
    TrafficLight trafficLight;
    std::vector<std::shared_ptr<Road>> roads;
    std::vector<std::shared_ptr<Vehicle>> allVehicles;
    double currentSimulationTime;

public:
    SimulationEngine();
    void initializeScenario();
    void step(double dt);

    double getTime() const { return currentSimulationTime; }
    const std::vector<std::shared_ptr<Vehicle>>& getAllVehicles() const { return allVehicles; }
    const TrafficLight& getTrafficLight() const { return trafficLight; }
};