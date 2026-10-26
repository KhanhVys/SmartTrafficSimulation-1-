// Intersection.h
#pragma once
#include "Road.h"
#include <vector>
#include <memory>
class Intersection : public IRoadComponent {
public:
    void addRoad(std::shared_ptr<IRoadComponent> road) { roads.push_back(road); }
    float getDistanceToStopLine(float x, float y, Direction dir) const override;
private:
    std::vector<std::shared_ptr<IRoadComponent>> roads;
};