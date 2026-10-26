#pragma once
#include "ILightState.h"

class GreenState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Green; }
};
