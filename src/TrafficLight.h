// TrafficLight.h
#pragma once
#include "common/Types.h"
#include <memory>
class ILightState;
class TrafficLight {
public:
    TrafficLight();
    ~TrafficLight();
    void update(int currentTick);
    LightColor getColor(Direction direction, Movement movement) const;
private:
    int currentTick;
    std::unique_ptr<ILightState> nsStraight, nsTurn, ewStraight, ewTurn;
};