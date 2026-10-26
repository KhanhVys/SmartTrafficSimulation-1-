#pragma once
#include "common/Types.h"

// Đáp ứng yêu cầu số 6: Composite Pattern
class IRoadComponent {
public:
    virtual ~IRoadComponent() = default;
    virtual float getDistanceToStopLine(float x, float y, Direction dir) const = 0;
};

class Intersection : public IRoadComponent {
public:
    float getDistanceToStopLine(float x, float y, Direction dir) const override;
};