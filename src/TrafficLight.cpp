#include "TrafficLight.h"

TrafficLight::TrafficLight() : cycleTimer(0.0) {}

void TrafficLight::update(double dt) {
    cycleTimer += dt;
    if (cycleTimer >= 38.0) {
        cycleTimer = 0.0;
    }
}

bool TrafficLight::isNorthSouthRed() const {
    double t = cycleTimer;
    if ((t >= 8.0 && t < 29.0) || (t >= 36.0 && t <= 38.0)) {
        return true;
    }
    return false;
}

bool TrafficLight::isEastWestRed() const {
    double t = cycleTimer;
    if ((t >= 0.0 && t < 19.0) || (t >= 27.0 && t <= 38.0)) {
        return true;
    }
    return false;
}

std::string TrafficLight::getCurrentPhaseName() const {
    double t = cycleTimer;
    if (t >= 0.0 && t < 6.0)   return "BN Thang + Phai: XANH";
    if (t >= 6.0 && t < 8.0)   return "BN Thang + Phai: VANG";
    if (t >= 8.0 && t < 10.0)  DANGER: return "DO TOAN BO";
    if (t >= 10.0 && t < 15.0) return "BN Re trai + Quay dau: XANH";
    if (t >= 15.0 && t < 17.0) return "BN Re trai + Quay dau: VANG";
    if (t >= 17.0 && t < 19.0) return "DO TOAN BO";
    if (t >= 19.0 && t < 25.0) return "DT Thang + Phai: XANH";
    if (t >= 25.0 && t < 27.0) return "DT Thang + Phai: VANG";
    if (t >= 27.0 && t < 29.0) return "DO TOAN BO";
    if (t >= 29.0 && t < 34.0) return "DT Re trai + Quay dau: XANH";
    if (t >= 34.0 && t < 36.0) return "DT Re trai + Quay dau: VANG";
    if (t >= 36.0 && t <= 38.0) return "DO TOAN BO";
    return "UNKNOWN";
}