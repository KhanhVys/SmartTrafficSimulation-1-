#include "TrafficLight.h"
#include "common/Constants.h"

class ILightState { 
public: 
    virtual ~ILightState() = default; 
    virtual LightColor getColor() const = 0; 
};
class RedState : public ILightState { public: LightColor getColor() const override { return LightColor::Red; } };
class YellowState : public ILightState { public: LightColor getColor() const override { return LightColor::Yellow; } };
class GreenState : public ILightState { public: LightColor getColor() const override { return LightColor::Green; } };

TrafficLight::TrafficLight() : currentTick(0) {
    nsStraight = std::make_unique<GreenState>(); 
    nsTurn = std::make_unique<RedState>();
    ewStraight = std::make_unique<RedState>(); 
    ewTurn = std::make_unique<RedState>();
}

TrafficLight::~TrafficLight() = default;

void TrafficLight::update(int tick) {
    currentTick = tick;
    int t = tick % CYCLE_TICKS;
    
    // Sử dụng if/else thay vì toán tử 3 ngôi ? : để tránh lỗi incompatible types
    // 1. BN Thẳng + Phải
    if (t < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS) 
        nsStraight = std::make_unique<GreenState>();
    else if (t < NS_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) 
        nsStraight = std::make_unique<YellowState>();
    else 
        nsStraight = std::make_unique<RedState>();

    // 2. BN Trái + Quay đầu
    if (t >= NS_TURN_START && t < NS_TURN_START + TURN_GREEN_TICKS) 
        nsTurn = std::make_unique<GreenState>();
    else if (t >= NS_TURN_START + TURN_GREEN_TICKS && t < NS_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) 
        nsTurn = std::make_unique<YellowState>();
    else 
        nsTurn = std::make_unique<RedState>();

    // 3. ĐT Thẳng + Phải
    if (t >= EW_STRAIGHT_START && t < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS) 
        ewStraight = std::make_unique<GreenState>();
    else if (t >= EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS && t < EW_STRAIGHT_START + STRAIGHT_GREEN_TICKS + YELLOW_TICKS) 
        ewStraight = std::make_unique<YellowState>();
    else 
        ewStraight = std::make_unique<RedState>();

    // 4. ĐT Trái + Quay đầu
    if (t >= EW_TURN_START && t < EW_TURN_START + TURN_GREEN_TICKS) 
        ewTurn = std::make_unique<GreenState>();
    else if (t >= EW_TURN_START + TURN_GREEN_TICKS && t < EW_TURN_START + TURN_GREEN_TICKS + YELLOW_TICKS) 
        ewTurn = std::make_unique<YellowState>();
    else 
        ewTurn = std::make_unique<RedState>();
}

LightColor TrafficLight::getColor(Direction dir, Movement mov) const {
    bool isNS = (dir == Direction::North || dir == Direction::South);
    bool isSR = (mov == Movement::Straight || mov == Movement::Right);
    
    if (isNS && isSR) return nsStraight->getColor();
    if (isNS && !isSR) return nsTurn->getColor();
    if (!isNS && isSR) return ewStraight->getColor();
    return ewTurn->getColor();
}