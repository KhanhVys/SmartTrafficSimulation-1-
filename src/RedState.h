#pragma once
#include "ILightState.h"

class RedState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Red; }
};
