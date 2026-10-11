#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <string>

class TrafficLight {
private:
    double cycleTimer;

public:
    TrafficLight();

    void update(double dt);
    
    bool isNorthSouthRed() const;
    bool isEastWestRed() const;
    
    std::string getCurrentPhaseName() const;
    double getCycleTimer() const { return cycleTimer; }
};

#endif