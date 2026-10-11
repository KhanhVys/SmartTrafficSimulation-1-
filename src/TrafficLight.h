#pragma once
#include <memory>
#include "Enums.h"
#include "ILightState.h"

class TrafficLight {
private:
    double elapsedTime;

    SignalColor bnStraightRightColor;
    SignalColor bnLeftUTurnColor;
    SignalColor dtStraightRightColor; 
    SignalColor dtLeftUTurnColor;

public:
    TrafficLight();
    void update(double dt);
    bool isRedFor(Direction dir, TurnType turn) const;

    SignalColor getBNStraightRight() const { return bnStraightRightColor; }
    SignalColor getBNLeftUTurn() const { return bnLeftUTurnColor; }
    SignalColor getDTStraightRight() const { return dtStraightRightColor; }
    SignalColor getDTLeftUTurn() const { return dtLeftUTurnColor; }
    double getElapsedTime() const { return elapsedTime; }
};