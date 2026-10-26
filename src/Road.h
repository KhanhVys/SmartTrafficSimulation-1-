#pragma once
#include "IRoadComponent.h"

class Road : public IRoadComponent {
public:
    Road() = default;
    float getDistanceToStopLine(float x, float y, Direction dir) const override {
        (void)x; (void)y; (void)dir; // Thêm dòng này để tắt cảnh báo
        return 9999.0f; // Đường đơn không có vạch dừng ngã tư
    }
};