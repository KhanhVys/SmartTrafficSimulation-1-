#pragma once
#include <string>
#include "Enums.h"

class ILightState {
public:
    virtual ~ILightState() = default;
    virtual SignalColor getColor() const = 0;
    virtual std::string getColorName() const = 0;
};

class RedState : public ILightState {
public:
    SignalColor getColor() const override { return SignalColor::Red; }
    std::string getColorName() const override { return "RED"; }
};

class YellowState : public ILightState {
public:
    SignalColor getColor() const override { return SignalColor::Yellow; }
    std::string getColorName() const override { return "YELLOW"; }
};

class GreenState : public ILightState {
public:
    SignalColor getColor() const override { return SignalColor::Green; }
    std::string getColorName() const override { return "GREEN"; }
};