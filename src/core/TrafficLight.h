#pragma once

#include <array>
#include <memory>

#include "common/Types.h"
#include "core/ILightState.h"

// 4 nhóm đèn theo biên bản họp. Rẽ phải đi chung đèn với đi thẳng.
enum class LightGroup { NorthSouthStraight, NorthSouthTurn, EastWestStraight, EastWestTurn };

constexpr int LIGHT_GROUP_COUNT = 4;

// Bộ đèn của ngã tư. Mỗi nhóm đèn giữ một ILightState (Red/Yellow/Green).
// Thời gian tính bằng tick nguyên, lấy từ common/Constants.h.
// Một nơi ghi duy nhất: luồng mô phỏng gọi update(); các nơi khác chỉ gọi getter.
class TrafficLight {
public:
    TrafficLight();

    // Gọi mỗi tick với tick hiện tại của đồng hồ mô phỏng.
    void update(long long currentTick);

    LightColor getColor(Direction arm, Movement movement) const;
    LightColor getGroupColor(LightGroup group) const;
    const char* getGroupStateName(LightGroup group) const;
    int getTickInCycle() const;

    static LightGroup getGroupOf(Direction arm, Movement movement);

private:
    std::array<LightSchedule, LIGHT_GROUP_COUNT> schedules;
    std::array<std::unique_ptr<ILightState>, LIGHT_GROUP_COUNT> states;
    int tickInCycle = 0;
};
