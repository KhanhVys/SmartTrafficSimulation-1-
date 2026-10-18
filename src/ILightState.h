#pragma once
#include <string>
#include "Types.h"

class ILightState {
public:
    virtual ~ILightState() = default;
    virtual LightColor getColor() const = 0;
    virtual std::string getColorName() const = 0;
};

class RedState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Red; }
    std::string getColorName() const override { return "RED"; }
};

class YellowState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Yellow; }
    std::string getColorName() const override { return "YELLOW"; }
};

class GreenState : public ILightState {
public:
    LightColor getColor() const override { return LightColor::Green; }
    std::string getColorName() const override { return "GREEN"; }
};