#include "core/RedState.h"

#include "core/GreenState.h"

LightColor RedState::getColor() const { return LightColor::Red; }
const char* RedState::getName() const { return "RedState"; }

// Đỏ kéo dài tới khi tới lượt nhóm này xanh. Khoảng "đỏ toàn bộ" giữa các pha
// tự có do lịch của các nhóm không chồng lên nhau.
std::unique_ptr<ILightState> RedState::update(const LightSchedule& schedule, int tickInCycle) {
    if (getTicksSinceGreen(schedule, tickInCycle) < schedule.greenTicks) {
        return std::make_unique<GreenState>();
    }
    return nullptr;
}
