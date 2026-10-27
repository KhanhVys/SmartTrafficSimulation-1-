#include "core/TrafficLight.h"
#include <utility>

#include "common/Constants.h"
#include "core/GreenState.h"
#include "core/RedState.h"
#include "core/YellowState.h"

namespace {

std::size_t groupIndex(LightGroup group) { return static_cast<std::size_t>(group); }

std::unique_ptr<ILightState> createInitialState(const LightSchedule& schedule, int tickInCycle) {
    const int elapsed = getTicksSinceGreen(schedule, tickInCycle);
    if (elapsed < schedule.greenTicks) {
        return std::make_unique<GreenState>();
    }
    if (elapsed < schedule.greenTicks + YELLOW_TICKS) {
        return std::make_unique<YellowState>();
    }
    return std::make_unique<RedState>();
}

}  // namespace

TrafficLight::TrafficLight() {
    schedules[groupIndex(LightGroup::NorthSouthStraight)] = {NS_STRAIGHT_START, STRAIGHT_GREEN_TICKS};
    schedules[groupIndex(LightGroup::NorthSouthTurn)] = {NS_TURN_START, TURN_GREEN_TICKS};
    schedules[groupIndex(LightGroup::EastWestStraight)] = {EW_STRAIGHT_START, STRAIGHT_GREEN_TICKS};
    schedules[groupIndex(LightGroup::EastWestTurn)] = {EW_TURN_START, TURN_GREEN_TICKS};
    for (std::size_t i = 0; i < states.size(); ++i) {
        states[i] = createInitialState(schedules[i], tickInCycle);
    }
}

void TrafficLight::update(long long currentTick) {
    tickInCycle = static_cast<int>(((currentTick % CYCLE_TICKS) + CYCLE_TICKS) % CYCLE_TICKS);
    for (std::size_t i = 0; i < states.size(); ++i) {
        std::unique_ptr<ILightState> next = states[i]->update(schedules[i], tickInCycle);
        if (next) {
            states[i] = std::move(next);
        }
    }
}

LightColor TrafficLight::getColor(Direction arm, Movement movement) const {
    return getGroupColor(getGroupOf(arm, movement));
}

LightColor TrafficLight::getGroupColor(LightGroup group) const {
    return states[groupIndex(group)]->getColor();
}

const char* TrafficLight::getGroupStateName(LightGroup group) const {
    return states[groupIndex(group)]->getName();
}

int TrafficLight::getTickInCycle() const { return tickInCycle; }

LightGroup TrafficLight::getGroupOf(Direction arm, Movement movement) {
    const bool northSouth = (arm == Direction::North || arm == Direction::South);
    const bool turnGroup = (movement == Movement::Left || movement == Movement::UTurn);
    if (northSouth) {
        return turnGroup ? LightGroup::NorthSouthTurn : LightGroup::NorthSouthStraight;
    }
    return turnGroup ? LightGroup::EastWestTurn : LightGroup::EastWestStraight;
}
