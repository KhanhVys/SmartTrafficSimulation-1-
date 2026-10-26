#include "TrafficLight.h"
#include "RedState.h"
#include "YellowState.h"
#include "GreenState.h"

TrafficLight::TrafficLight() : currentTick(0) { setState(std::make_unique<RedState>()); }
TrafficLight::~TrafficLight() = default;

void TrafficLight::setState(std::unique_ptr<ILightState> newState) { currentState = std::move(newState); }

void TrafficLight::update(int currentTick) {
    this->currentTick = currentTick;
    int tickInCycle = currentTick % Config::TOTAL_TICKS_PER_CYCLE;
    int second = tickInCycle / Config::TICKS_PER_SECOND;

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
    int tickInCycle = currentTick % Config::TOTAL_TICKS_PER_CYCLE;
    return calculateColor(tickInCycle, direction, movement);
}

bool TrafficLight::isRedFor(Direction direction, Movement movement) const {
    return getColor(direction, movement) == LightColor::Red;
}

LightColor TrafficLight::calculateColor(int tickInCycle, Direction dir, Movement mov) const {
    int second = tickInCycle / Config::TICKS_PER_SECOND;
    bool isNorthSouth  = (dir == Direction::North || dir == Direction::South);
    bool isEastWest    = (dir == Direction::East  || dir == Direction::West);
    bool isStraightR   = (mov == Movement::GoStraight || mov == Movement::TurnRight);
    bool isLeftU       = (mov == Movement::TurnLeft   || mov == Movement::UTurn);

    if (second >= 0  && second <= 5  && isNorthSouth && isStraightR) return LightColor::Green;
    if (second >= 6  && second <= 7  && isNorthSouth && isStraightR) return LightColor::Yellow;
    if (second >= 10 && second <= 14 && isNorthSouth && isLeftU)     return LightColor::Green;
    if (second >= 15 && second <= 16 && isNorthSouth && isLeftU)     return LightColor::Yellow;
    if (second >= 19 && second <= 24 && isEastWest   && isStraightR) return LightColor::Green;
    if (second >= 25 && second <= 26 && isEastWest   && isStraightR) return LightColor::Yellow;
    if (second >= 29 && second <= 33 && isEastWest   && isLeftU)     return LightColor::Green;
    if (second >= 34 && second <= 35 && isEastWest   && isLeftU)     return LightColor::Yellow;
    return LightColor::Red;
}