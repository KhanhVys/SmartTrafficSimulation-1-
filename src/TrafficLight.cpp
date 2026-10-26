#include "TrafficLight.h"
#include "common/Constants.h"

// --- Định nghĩa các State ---
class ILightState {
public:
    virtual ~ILightState() = default;
    virtual LightColor getColor() const = 0;
};
class RedState : public ILightState { public: LightColor getColor() const override { return LightColor::Red; } };
class YellowState : public ILightState { public: LightColor getColor() const override { return LightColor::Yellow; } };
class GreenState : public ILightState { public: LightColor getColor() const override { return LightColor::Green; } };

TrafficLight::TrafficLight() {
    nsStraightState = std::make_unique<GreenState>();
    nsTurnState = std::make_unique<RedState>();
    ewStraightState = std::make_unique<RedState>();
    ewTurnState = std::make_unique<RedState>();
}

TrafficLight::~TrafficLight() = default;

void TrafficLight::update(int tick) {
    int t = tick % CYCLE_TICKS;
    
    // BN Thẳng + Phải
    if (t < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS) nsStraightState = std::make_unique<GreenState>();
    else if (t < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) nsStraightState = std::make_unique<YellowState>();
    else nsStraightState = std::make_unique<RedState>();

    // BN Trái + Quay đầu
    if (t >= NS_TURN_START && t < NS_TURN_START + TURN_GREEN_TICKS) nsTurnState = std::make_unique<GreenState>();
    else if (t >= NS_TURN_START + TURN_GREEN_TICKS && t < NS_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) nsTurnState = std::make_unique<YellowState>();
    else nsTurnState = std::make_unique<RedState>();

    // ĐT Thẳng + Phải
    if (t >= EW_STRAIGHT_START && t < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS) ewStraightState = std::make_unique<GreenState>();
    else if (t >= EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS && t < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) ewStraightState = std::make_unique<YellowState>();
    else ewStraightState = std::make_unique<RedState>();

    // ĐT Trái + Quay đầu
    if (t >= EW_TURN_START && t < EW_TURN_START + TURN_GREEN_TICKS) ewTurnState = std::make_unique<GreenState>();
    else if (t >= EW_TURN_START + TURN_GREEN_TICKS && t < EW_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) ewTurnState = std::make_unique<YellowState>();
    else ewTurnState = std::make_unique<RedState>();
}

LightColor TrafficLight::getColor(Direction dir, Movement mov) const {
    bool isNS = (dir == Direction::North || dir == Direction::South);
    bool isSR = (mov == Movement::Straight || mov == Movement::Right);
    
    if (isNS && isSR) return nsStraightState->getColor();
    if (isNS && !isSR) return nsTurnState->getColor();
    if (!isNS && isSR) return ewStraightState->getColor();
    return ewTurnState->getColor();
}

bool TrafficLight::isRedFor(Direction direction, Movement movement) const {
    return getColor(direction, movement) == LightColor::Red;
}