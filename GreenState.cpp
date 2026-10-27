#include "core/GreenState.h"

#include "core/YellowState.h"

LightColor GreenState::getColor() const { return LightColor::Green; }
const char* GreenState::getName() const { return "GreenState"; }

std::unique_ptr<ILightState> GreenState::update(const LightSchedule& schedule, int tickInCycle) {
    if (getTicksSinceGreen(schedule, tickInCycle) >= schedule.greenTicks) {
        return std::make_unique<YellowState>();
    }
    return nullptr;
}
