#pragma once
#include "common/Types.h"

class ILightState {
public:
    virtual ~ILightState() = default;
    virtual LightColor getColor() const = 0;
};