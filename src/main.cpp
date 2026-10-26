#include "Vehicle.h"
#include "TrafficLight.h"
#include "IVehicleState.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <thread>   
#include <chrono>  
class TrafficRule {
public:
    void applyRules(Vehicle& v, TrafficLight& light) {
        float dist = v.getDistanceToStopLine();
        LightColor c = light.getColor(v.getDirection(), v.getMovement());

        if (dist > 0.0f) {
            if (c == LightColor::Red || c == LightColor::Yellow) {
                v.setBrakeForLight(true);
                v.setCanProceed(false);
            } else {
                v.setBrakeForLight(false);
                v.setCanProceed(true);
            }
        } else {
            v.setBrakeForLight(false);
            v.setCanProceed(true);
        }
    }
};

const float INTERSECTION_X = 400.0f;
const float INTERSECTION_Y = 300.0f;
const float STOP_LINE_OFFSET = 50.0f; 

float calcDistToStopLine(float x, float y, Direction dir) {
    switch (dir) {
        case Direction::South: return (INTERSECTION_Y - STOP_LINE_OFFSET) - y;
        case Direction::North: return y - (INTERSECTION_Y + STOP_LINE_OFFSET);
        case Direction::East:  return (INTERSECTION_X - STOP_LINE_OFFSET) - x;
        case Direction::West:  return x - (INTERSECTION_X + STOP_LINE_OFFSET);
    }
    return 1000.0f;
}

Vehicle* findVehicleAhead(Vehicle* cur, std::vector<std::unique_ptr<Vehicle>>& vs) {
    Vehicle* best = nullptr; float bestDist = 1e9f;
    for (auto& v : vs) {
        if (v.get() == cur) continue;
        if (v->getDirection() != cur->getDirection()) continue;
        if (v->getMovement()  != cur->getMovement())  continue;
        float dx = v->getX() - cur->getX();
        float dy = v->getY() - cur->getY();
        float d = std::sqrt(dx*dx + dy*dy);
        bool ahead = false;
        switch (cur->getDirection()) {
            case Direction::South: ahead = v->getY() > cur->getY(); break;
            case Direction::North: ahead = v->getY() < cur->getY(); break;
            case Direction::East:  ahead = v->getX() > cur->getX(); break;
            case Direction::West:  ahead = v->getX() < cur->getX(); break;
        }
        if (ahead && d < bestDist) { bestDist = d; best = v.get(); }
    }
    return best;
}

int main() {
    TrafficLight light;
    TrafficRule rule;
    std::vector<std::unique_ptr<Vehicle>> vehicles;

    vehicles.push_back(std::make_unique<Vehicle>(1, Direction::South, Movement::GoStraight, 350, 50));
    vehicles.push_back(std::make_unique<Vehicle>(2, Direction::South, Movement::GoStraight, 350, 130));
    vehicles.push_back(std::make_unique<Vehicle>(3, Direction::South, Movement::GoStraight, 350, 210));
    vehicles.push_back(std::make_unique<Vehicle>(4, Direction::North, Movement::GoStraight, 450, 550));
    vehicles.push_back(std::make_unique<Vehicle>(5, Direction::North, Movement::GoStraight, 450, 470));
    vehicles.push_back(std::make_unique<Vehicle>(6, Direction::North, Movement::GoStraight, 450, 390));
    vehicles.push_back(std::make_unique<Vehicle>(7, Direction::East,  Movement::GoStraight, 50,  250));
    vehicles.push_back(std::make_unique<Vehicle>(8, Direction::East,  Movement::GoStraight, 130, 250));
    vehicles.push_back(std::make_unique<Vehicle>(9, Direction::East,  Movement::GoStraight, 210, 250));
    vehicles.push_back(std::make_unique<Vehicle>(10, Direction::West, Movement::GoStraight, 750, 350));
    vehicles.push_back(std::make_unique<Vehicle>(11, Direction::West, Movement::GoStraight, 670, 350));
    vehicles.push_back(std::make_unique<Vehicle>(12, Direction::West, Movement::GoStraight, 590, 350));
    vehicles.push_back(std::make_unique<Vehicle>(13, Direction::South, Movement::GoStraight, 350, 290));

    const int TOTAL_TICKS = 50 * 40;
    const float DT = 0.02f;

    auto nextTickTime = std::chrono::steady_clock::now();

    for (int tick = 0; tick <= TOTAL_TICKS; tick++) {
        light.update(tick);
        for (auto& v : vehicles) {
            v->setDistanceToStopLine(calcDistToStopLine(v->getX(), v->getY(), v->getDirection()));
            v->setVehicleAhead(findVehicleAhead(v.get(), vehicles));
        }
        for (auto& v : vehicles) rule.applyRules(*v, light);
        for (auto& v : vehicles) v->update(DT);

        if (tick % 50 == 0) {
            std::cout << "===== Giay " << (tick / 50) << " =====\n";
            for (auto& v : vehicles) {
                std::cout << " Xe " << std::setw(2) << v->getId()
                          << " | Pos(" << std::setw(4) << (int)v->getX()
                          << "," << std::setw(4) << (int)v->getY() << ")"
                          << " | Spd=" << std::fixed << std::setprecision(1) << v->getSpeed()
                          << " | DistLine=" << std::setw(5) << (int)v->getDistanceToStopLine()
                          << " | State=" << std::setw(13) << (v->getState() ? v->getState()->getName() : "?")
                          << " | Brake=" << (v->shouldBrake() ? "Y" : "N")
                          << "\n";
            }
            std::cout << "\n";
        }

        nextTickTime += std::chrono::milliseconds(20);

        std::this_thread::sleep_until(nextTickTime);
    }
    
    std::cout << "Da hoan thanh mo phong 40 giay!\n";
    return 0;
}