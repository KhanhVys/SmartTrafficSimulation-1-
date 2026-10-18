#include "Road.h"
#include <iostream>

Road::Road(Direction dir, int lanes) : direction(dir), laneCount(lanes) {}

void Road::addVehicle(std::shared_ptr<Vehicle> vehicle) {
    vehicleList.push_back(vehicle);
}

void Road::updateVehicles(double dt, bool isRedLightForLane, int laneIndex) {
    for (size_t i = 0; i < vehicleList.size(); ++i) {
        auto& v = vehicleList[i];
        if (v->getLaneIndex() == laneIndex) {
            bool clearAhead = true;
            if (i > 0) {
                double distanceToPreceding = v->getPosition() - vehicleList[i - 1]->getPosition();
                if (distanceToPreceding > 0 && distanceToPreceding < 12.0) {
                    clearAhead = false;
                }
            }
            v->update(dt, v->getPosition(), isRedLightForLane, clearAhead);
        }
    }
}

void Road::displayInfo() const {
    std::cout << "Road with " << laneCount << " lanes, containing " << vehicleList.size() << " vehicles.\n";
}