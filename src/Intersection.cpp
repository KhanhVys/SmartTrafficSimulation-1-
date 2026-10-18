#include "Intersection.h"

Intersection::Intersection() {}

void Intersection::addRoad(std::shared_ptr<Road> road) {
    roadNetwork.push_back(road);
}

void Intersection::updateIntersection(double dt) {
    trafficLight.update(dt);
    for (auto& road : roadNetwork) {
        Direction dir = road->getDirection();
        for (int lane = 0; lane < road->getLaneCount(); ++lane) {
            Movement move = (lane == 0) ? Movement::Left : Movement::Straight;
            bool isRed = trafficLight.isRedFor(dir, move);
            road->updateVehicles(dt, isRed, lane);
        }
    }
}
