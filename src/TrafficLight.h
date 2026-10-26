#pragma once
#include "common/Types.h" // Đã đổi include
#include <memory>

class ILightState;

class TrafficLight {
public:
    TrafficLight();
    ~TrafficLight();
    void update(int currentTick);
    LightColor getColor(Direction direction, Movement movement) const;
    bool isRedFor(Direction direction, Movement movement) const;

private:
    int currentTick;
    std::unique_ptr<ILightState> currentState;
    LightColor calculateColor(int tickInCycle, Direction direction, Movement movement) const;
    void setState(std::unique_ptr<ILightState> newState);
};