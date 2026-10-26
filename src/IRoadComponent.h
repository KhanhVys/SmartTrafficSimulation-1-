#pragma once
#include "common/Types.h"

class IRoadComponent {
public:
    virtual ~IRoadComponent() = default;
    virtual float getDistanceToStopLine(float x, float y, Direction dir) const = 0;
};