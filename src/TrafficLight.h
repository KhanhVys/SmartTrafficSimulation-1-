#pragma once
#include <memory>
#include "Types.h"
#include "ILightState.h"

class TrafficLight {
private:
    double elapsedTime; 

    LightColor bnStraightRightColor;
    LightColor bnLeftUTurnColor;
    LightColor dtStraightRightColor;
    LightColor dtLeftUTurnColor;

public:
    TrafficLight();
    
    void update(double dt);
    bool isRedFor(Direction dir, Movement move) const;

    LightColor getBnStraightRightColor() const { return bnStraightRightColor; }
    LightColor getBnLeftUTurnColor() const { return bnLeftUTurnColor; }
    LightColor getDtStraightRightColor() const { return dtStraightRightColor; }
    LightColor getDtLeftUTurnColor() const { return dtLeftUTurnColor; }
    double getElapsedTime() const { return elapsedTime; }
};