#pragma once

#include "core/ILightState.h"

class YellowState : public ILightState {
public:
    LightColor getColor() const override;
    const char* getName() const override;
    std::unique_ptr<ILightState> update(const LightSchedule& schedule, int tickInCycle) override;
};
