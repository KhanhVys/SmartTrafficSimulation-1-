#pragma once

#include <memory>

#include "common/Constants.h"
#include "common/Types.h"

// Lịch của một nhóm đèn: bắt đầu xanh ở tick nào trong chu kỳ và xanh bao lâu (đơn vị tick).
struct LightSchedule {
    int greenStart = 0;
    int greenTicks = 0;
};

// Số tick đã trôi qua kể từ lúc nhóm đèn này bắt đầu xanh (tính vòng theo chu kỳ).
inline int getTicksSinceGreen(const LightSchedule& schedule, int tickInCycle) {
    return (tickInCycle - schedule.greenStart + CYCLE_TICKS) % CYCLE_TICKS;
}

// State Pattern cho đèn: mỗi state biết khi nào phải chuyển sang state kế tiếp.
// update() trả về state mới, hoặc nullptr nếu giữ nguyên. Không tự thay state trong
// lúc đang chạy để tránh huỷ chính đối tượng đang thực thi.
class ILightState {
public:
    virtual ~ILightState() = default;

    virtual LightColor getColor() const = 0;
    virtual const char* getName() const = 0;
    virtual std::unique_ptr<ILightState> update(const LightSchedule& schedule, int tickInCycle) = 0;
};
