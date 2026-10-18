#include "TrafficLight.h"

TrafficLight::TrafficLight() : elapsedTime(0.0) {
    update(0.0);
}

void TrafficLight::update(double dt) {
    elapsedTime += dt;
    if (elapsedTime >= 38.0) {
        elapsedTime -= 38.0; 
    }

    bnStraightRightColor = LightColor::Red;
    bnLeftUTurnColor     = LightColor::Red;
    dtStraightRightColor = LightColor::Red;
    dtLeftUTurnColor     = LightColor::Red;

    if (elapsedTime >= 0.0 && elapsedTime < 6.0) {
        bnStraightRightColor = LightColor::Green;
    } else if (elapsedTime >= 6.0 && elapsedTime < 8.0) {
        bnStraightRightColor = LightColor::Yellow;
    } else if (elapsedTime >= 10.0 && elapsedTime < 15.0) {
        bnLeftUTurnColor = LightColor::Green;
    } else if (elapsedTime >= 15.0 && elapsedTime < 17.0) {
        bnLeftUTurnColor = LightColor::Yellow;
    } else if (elapsedTime >= 19.0 && elapsedTime < 25.0) {
        dtStraightRightColor = LightColor::Green;
    } else if (elapsedTime >= 25.0 && elapsedTime < 27.0) {
        dtStraightRightColor = LightColor::Yellow;
    } else if (elapsedTime >= 29.0 && elapsedTime < 34.0) {
        dtLeftUTurnColor = LightColor::Green;
    } else if (elapsedTime >= 34.0 && elapsedTime < 36.0) {
        dtLeftUTurnColor = LightColor::Yellow;
    }
}

bool TrafficLight::isRedFor(Direction dir, Movement move) const {
    LightColor activeColor = LightColor::Red;

    if (dir == Direction::North || dir == Direction::South) {
        if (move == Movement::Straight || move == Movement::Right) {
            activeColor = bnStraightRightColor;
        } else {
            activeColor = bnLeftUTurnColor;
        }
    } else {
        if (move == Movement::Straight || move == Movement::Right) {
            activeColor = dtStraightRightColor;
        } else {
            activeColor = dtLeftUTurnColor;
        }
    }

    return (activeColor == LightColor::Red || activeColor == LightColor::Yellow);
}
