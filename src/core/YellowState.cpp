#include "core/YellowState.h"

#include "core/RedState.h"

LightColor YellowState::getColor() const { return LightColor::Yellow; }
const char* YellowState::getName() const { return "YellowState"; }

std::unique_ptr<ILightState> YellowState::update(const LightSchedule& schedule, int tickInCycle) {
    if (getTicksSinceGreen(schedule, tickInCycle) >= schedule.greenTicks + YELLOW_TICKS) {
        return std::make_unique<RedState>();
    }
    return nullptr;
}
