#pragma once
#include "IRoadComponent.h"
#include <vector>
#include <memory>

class Intersection : public IRoadComponent {
public:
    Intersection() = default;
    
    // Hàm đặc trưng của Composite Pattern
    void addRoad(std::shared_ptr<IRoadComponent> road) {
        roads.push_back(road);
    }
    
    float getDistanceToStopLine(float x, float y, Direction dir) const override;
    
private:
    std::vector<std::shared_ptr<IRoadComponent>> roads;
};