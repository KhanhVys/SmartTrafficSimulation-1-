#pragma once
#include "ILightState.h"
class YellowState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Yellow; }
};