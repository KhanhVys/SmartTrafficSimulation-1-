#pragma once

#include "common/Types.h"

class TrafficRule
{
public:
    static bool shouldStop(
        LightColor lightColor,
        bool hasCrossedStopLine);
};