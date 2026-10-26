#pragma once
#include "CommonTypes.h"

class ILightState {
public:
    virtual ~ILightState() = default;
    virtual LightColor getColor() const = 0;
};