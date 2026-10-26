#include "TrafficLight.h"
#include "States.h"
#include "Constants.h"

TrafficLight::TrafficLight() : currentTick(0) { setState(std::make_unique<RedState>()); }
TrafficLight::~TrafficLight() = default;

void TrafficLight::setState(std::unique_ptr<ILightState> newState) { currentState = std::move(newState); }

void TrafficLight::update(int tick) {
    currentTick = tick;
    int tickInCycle = currentTick % CYCLE_TICKS;
    int second = tickInCycle / TICKS_PER_SECOND;

    // Vẫn giữ logic setState để quản lý trạng thái tổng thể (nếu GUI cần hiển thị)
    if      (second >= 0  && second <= 5)  setState(std::make_unique<GreenState>());
    else if (second >= 6  && second <= 7)  setState(std::make_unique<YellowState>());
    else if (second >= 8  && second <= 9)  setState(std::make_unique<RedState>());
    else if (second >= 10 && second <= 14) setState(std::make_unique<GreenState>());
    else if (second >= 15 && second <= 16) setState(std::make_unique<YellowState>());
    else if (second >= 17 && second <= 18) setState(std::make_unique<RedState>());
    else if (second >= 19 && second <= 24) setState(std::make_unique<GreenState>());
    else if (second >= 25 && second <= 26) setState(std::make_unique<YellowState>());
    else if (second >= 27 && second <= 28) setState(std::make_unique<RedState>());
    else if (second >= 29 && second <= 33) setState(std::make_unique<GreenState>());
    else if (second >= 34 && second <= 35) setState(std::make_unique<YellowState>());
    else                                   setState(std::make_unique<RedState>());
}

LightColor TrafficLight::getColor(Direction direction, Movement movement) const {
    int tickInCycle = currentTick % CYCLE_TICKS;
    return calculateColor(tickInCycle, direction, movement);
}

bool TrafficLight::isRedFor(Direction direction, Movement movement) const {
    return getColor(direction, movement) == LightColor::Red;
}

LightColor TrafficLight::calculateColor(int tick, Direction dir, Movement mov) const {
    bool isNS  = (dir == Direction::North || dir == Direction::South);
    bool isEW  = (dir == Direction::East  || dir == Direction::West);
    bool isStraightR = (mov == Movement::Straight || mov == Movement::Right);
    bool isLeftU     = (mov == Movement::Left     || mov == Movement::UTurn);

    // Sử dụng chính xác các mốc thời gian từ Constants.h của Leader
    if (isNS && isStraightR) {
        if (tick < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS) return LightColor::Green;
        if (tick < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isNS && isLeftU) {
        if (tick >= NS_TURN_START && tick < NS_TURN_START + TURN_GREEN_TICKS) return LightColor::Green;
        if (tick >= NS_TURN_START + TURN_GREEN_TICKS && tick < NS_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isEW && isStraightR) {
        if (tick >= EW_STRAIGHT_START && tick < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS) return LightColor::Green;
        if (tick >= EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS && tick < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }
    if (isEW && isLeftU) {
        if (tick >= EW_TURN_START && tick < EW_TURN_START + TURN_GREEN_TICKS) return LightColor::Green;
        if (tick >= EW_TURN_START + TURN_GREEN_TICKS && tick < EW_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) return LightColor::Yellow;
    }

    return LightColor::Red;
}