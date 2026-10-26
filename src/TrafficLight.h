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
    bool isRedFor(Direction direction, Movement movement) const;

private:
    // State cho 4 nhóm đèn: BN-Thẳng, BN-Rẽ, ĐT-Thẳng, ĐT-Rẽ
    std::unique_ptr<ILightState> nsStraightState;
    std::unique_ptr<ILightState> nsTurnState;
    std::unique_ptr<ILightState> ewStraightState;
    std::unique_ptr<ILightState> ewTurnState;
};