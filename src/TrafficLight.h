#pragma once
#include "common/Types.h"

class TrafficLight {
public:
    TrafficLight();
    ~TrafficLight();
    void update(int currentTick);
    LightColor getColor(Direction direction, Movement movement) const;
    bool isRedFor(Direction direction, Movement movement) const;

private:
    int currentTick;
    LightColor calculateColor(int tickInCycle, Direction direction, Movement movement) const;
};