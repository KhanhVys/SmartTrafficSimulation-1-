
#pragma once

#include "common/Types.h"

enum class TrafficPhase
{
    NorthSouthStraightRight,
    NorthSouthLeftUTurn,
    EastWestStraightRight,
    EastWestLeftUTurn,
    AllRed
};

class TrafficController
{
public:
    static constexpr int TICKS_PER_SECOND = 50;
    static constexpr int CYCLE_LENGTH_SECONDS = 38;
    static constexpr int CYCLE_LENGTH_TICKS =
        TICKS_PER_SECOND * CYCLE_LENGTH_SECONDS;

    TrafficController();

    // Tiến simulation thêm một tick.
    void update();

    int getCurrentTick() const;
    int getCurrentSecond() const;

    // Trả về pha đang hoạt động, bao gồm khoảng chuyển AllRed.
    TrafficPhase getCurrentPhase() const;

    // Trả về màu đèn của nhóm pha được yêu cầu.
    LightColor getSignalState(TrafficPhase phase) const;

private:
    int currentTick;
};
