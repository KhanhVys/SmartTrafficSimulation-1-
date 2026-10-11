#include "TrafficLight.h"

TrafficLight::TrafficLight() : elapsedTime(0.0) {
    update(0.0);
}

void TrafficLight::update(double dt) {
    elapsedTime += dt;
    if (elapsedTime >= 38.0) {
        elapsedTime -= 38.0;
    }

    bnStraightRightColor = SignalColor::Red;
    bnLeftUTurnColor     = SignalColor::Red;
    dtStraightRightColor = SignalColor::Red;
    dtLeftUTurnColor     = SignalColor::Red;

    if (elapsedTime >= 0.0 && elapsedTime < 6.0) {
        bnStraightRightColor = SignalColor::Green;
    } else if (elapsedTime >= 6.0 && elapsedTime < 8.0) {
        bnStraightRightColor = SignalColor::Yellow;
    } else if (elapsedTime >= 8.0 && elapsedTime < 10.0) {
    } else if (elapsedTime >= 10.0 && elapsedTime < 15.0) {
        bnLeftUTurnColor = SignalColor::Green;
    } else if (elapsedTime >= 15.0 && elapsedTime < 17.0) {
        bnLeftUTurnColor = SignalColor::Yellow;
    } else if (elapsedTime >= 17.0 && elapsedTime < 19.0) {
    } else if (elapsedTime >= 19.0 && elapsedTime < 25.0) {
        dtStraightRightColor = SignalColor::Green;
    } else if (elapsedTime >= 25.0 && elapsedTime < 27.0) {
        dtStraightRightColor = SignalColor::Yellow;
    } else if (elapsedTime >= 27.0 && elapsedTime < 29.0) {
    } else if (elapsedTime >= 29.0 && elapsedTime < 34.0) {
        dtLeftUTurnColor = SignalColor::Green;
    } else if (elapsedTime >= 34.0 && elapsedTime < 36.0) {
        dtLeftUTurnColor = SignalColor::Yellow;
    } else if (elapsedTime >= 36.0 && elapsedTime < 38.0) {
    }
}

bool TrafficLight::isRedFor(Direction dir, TurnType turn) const {
    SignalColor activeColor = SignalColor::Red;

    if (dir == Direction::North || dir == Direction::South) {
        if (turn == TurnType::Straight || turn == TurnType::Right) {
            activeColor = bnStraightRightColor;
        } else {
            activeColor = bnLeftUTurnColor;
        }
    } else { // East, West
        if (turn == TurnType::Straight || turn == TurnType::Right) {
            activeColor = dtStraightRightColor;
        } else {
            activeColor = dtLeftUTurnColor;
        }
    }

    return (activeColor == SignalColor::Red || activeColor == SignalColor::Yellow);
}