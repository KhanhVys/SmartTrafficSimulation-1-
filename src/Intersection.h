#pragma once
#include <vector>
#include <memory>
#include "Road.h"
#include "TrafficLight.h"

class Intersection {
private:
    TrafficLight trafficLight;
    std::vector<std::shared_ptr<Road>> roadNetwork;

public:
    Intersection();

    void addRoad(std::shared_ptr<Road> road);
    void updateIntersection(double dt);

    TrafficLight& getTrafficLight() { return trafficLight; }
    const std::vector<std::shared_ptr<Road>>& getRoadNetwork() const { return roadNetwork; }
};