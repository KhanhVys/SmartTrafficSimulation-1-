
#pragma once

#include "common/Types.h"
#include "TrafficController.h"

enum class LaneType
{
    LeftUTurn,
    Straight,
    Right,
    StraightRight
};

class TrafficRule
{
public:
    // Xác định pha đèn cần tuân theo.
    static TrafficPhase getRequiredPhase(
        Direction direction,
        Movement movement);

    // Xác định loại làn tương ứng với hướng đi.
    static LaneType getLaneType(
        Direction direction,
        Movement movement);

    // Kiểm tra xe có được phép di chuyển hay không.
    static bool canMove(
        Direction direction,
        Movement movement,
        const TrafficController& controller);

    // Kiểm tra xe có phải dừng trước vạch dừng hay không.
    static bool shouldStop(
        Direction direction,
        Movement movement,
        const TrafficController& controller,
        bool hasCrossedStopLine);

    // Kiểm tra xe có cần giảm tốc khi đèn vàng hay không.
    static bool shouldSlowDown(
        Direction direction,
        Movement movement,
        const TrafficController& controller,
        bool hasCrossedStopLine);

    // Quyết định xe dừng hay đi dựa trên màu đèn và vị trí xe.
    static bool shouldStop(
        LightColor lightColor,
        bool hasCrossedStopLine);
};
