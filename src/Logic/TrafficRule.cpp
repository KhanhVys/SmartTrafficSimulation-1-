#include "Logic/TrafficRule.h"

bool TrafficRule::shouldStop(
    LightColor lightColor,
    bool hasCrossedStopLine)
{
    // Xe đã qua vạch dừng thì không dừng lại.
    if (hasCrossedStopLine)
    {
        return false;
    }

    // Đèn đỏ hoặc vàng: xe chưa qua vạch phải dừng.
    return lightColor == LightColor::Red ||
           lightColor == LightColor::Yellow;
}