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
            double minDistance = 9999.0;
            
            for (size_t j = 0; j < vehicleList.size(); ++j) {
                if (i == j) continue;
                auto& other = vehicleList[j];
                if (other->getLaneIndex() == laneIndex) {
                    double dist = v->getPosition() - other->getPosition();
                    
                    if (dist < 0) {
                        dist += 240.0; 
                    }
                    
                    if (dist > 0 && dist < minDistance) {
                        minDistance = dist;
                    }
                }
            }
            
            double safeDist = (v->getSpeed() * v->getSpeed()) / (2.0 * v->getDeceleration()) + 12.0;
            
            if (minDistance < safeDist) {
                clearAhead = false;
                if (minDistance < 5.0 && v->getSpeed() > 0) {
                    v->setSpeed(0.0);
                }
            }
            
            v->update(dt, v->getPosition(), isRedLightForLane, clearAhead);
        }
    }
}

void Road::displayInfo() const {
    std::cout << "Road with " << laneCount << " lanes, containing " << vehicleList.size() << " vehicles.\n";
}
