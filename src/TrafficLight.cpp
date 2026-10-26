#include "TrafficLight.h"
#include "common/Constants.h"

TrafficLight::TrafficLight() : currentTick(0) {}
TrafficLight::~TrafficLight() = default;

void TrafficLight::update(int tick) {
    currentTick = tick; // Không còn if/else hardcode
}

LightColor TrafficLight::getColor(Direction direction, Movement movement) const {
    return calculateColor(currentTick % CYCLE_TICKS, direction, movement);
}

bool TrafficLight::isRedFor(Direction direction, Movement movement) const {
    return getColor(direction, movement) == LightColor::Red;
}

LightColor TrafficLight::calculateColor(int tick, Direction dir, Movement mov) const {
    bool isNS = (dir == Direction::North || dir == Direction::South);
    bool isEW = (dir == Direction::East || dir == Direction::West);
    bool isSR = (mov == Movement::Straight || mov == Movement::Right);
    bool isLU = (mov == Movement::Left || mov == Movement::UTurn);

    if (isNS && isSR) {
        if (tick < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS) return LightColor::Green;
        if (tick < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isNS && isLU) {
        if (tick >= NS_TURN_START && tick < NS_TURN_START + TURN_GREEN_TICKS) return LightColor::Green;
        if (tick >= NS_TURN_START + TURN_GREEN_TICKS && tick < NS_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isEW && isSR) {
        if (tick >= EW_STRAIGHT_START && tick < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS) return LightColor::Green;
        if (tick >= EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS && tick < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isEW && isLU) {
        if (tick >= EW_TURN_START && tick < EW_TURN_START + TURN_GREEN_TICKS) return LightColor::Green;
        if (tick >= EW_TURN_START + TURN_GREEN_TICKS && tick < EW_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    return LightColor::Red;
}