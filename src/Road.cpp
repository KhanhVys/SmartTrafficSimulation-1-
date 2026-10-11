#include "Road.h"

Road::Road(Direction dir, int lanes) : direction(dir), laneCount(lanes) {}

void Road::addVehicle(std::shared_ptr<Vehicle> vehicle) {
    vehicles.push_back(vehicle);
}

void Road::updateVehicles(double dt, bool isRedLightForLane, int laneIndex) {
    for (size_t i = 0; i < vehicles.size(); ++i) {
        auto& v = vehicles[i];
        if (v->getLaneIndex() == laneIndex) {
            // Kiểm tra khoảng cách an toàn với xe ngay trước cùng làn
            bool clearAhead = true;
            if (i > 0) {
                // Giả lập check xe trước
                double distanceToPreceding = v->getPosition() - vehicles[i-1]->getPosition();
                if (distanceToPreceding > 0 && distanceToPreceding < 10.0) {
                    clearAhead = false;
                }
            }
            v->update(dt, v->getPosition(), isRedLightForLane, clearAhead);
        }
    }
}