#pragma once
#include "common/Types.h"

class IRoadComponent {
public:
    virtual ~IRoadComponent() = default;
    virtual float getDistanceToStopLine(float x, float y, Direction dir) const = 0;
};

class Road : public IRoadComponent {
public:
    float getDistanceToStopLine(float x, float y, Direction dir) const override {
        // Thêm dòng này để báo với trình biên dịch: "Tao biết tao không dùng, đừng cảnh báo nữa"
        (void)x; (void)y; (void)dir; 
        return 9999.0f;
    }
};