#include "Vehicle.h"
#include "TrafficLight.h"
#include "States.h"
#include "Constants.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <thread>   
#include <chrono>   

float calcDistToStopLine(float x, float y, Direction dir) {
    const float INTERSECTION_X = 400.0f;
    const float INTERSECTION_Y = 300.0f;
    const float STOP_LINE_OFFSET = 50.0f; 
    
    switch (dir) {
        case Direction::South: return (INTERSECTION_Y - STOP_LINE_OFFSET) - y;
        case Direction::North: return y - (INTERSECTION_Y + STOP_LINE_OFFSET);
        case Direction::East:  return (INTERSECTION_X - STOP_LINE_OFFSET) - x;
        case Direction::West:  return x - (INTERSECTION_X + STOP_LINE_OFFSET);
    }
    return 1000.0f;
}

int main() {
    TrafficLight light;
    std::vector<std::unique_ptr<Vehicle>> vehicles;

    // 13 xe test
    vehicles.push_back(std::make_unique<Vehicle>(1, Direction::South, Movement::Straight, 350, 50));
    vehicles.push_back(std::make_unique<Vehicle>(2, Direction::South, Movement::Straight, 350, 130));
    vehicles.push_back(std::make_unique<Vehicle>(3, Direction::South, Movement::Straight, 350, 210));
    vehicles.push_back(std::make_unique<Vehicle>(4, Direction::North, Movement::Straight, 450, 550));
    vehicles.push_back(std::make_unique<Vehicle>(5, Direction::North, Movement::Straight, 450, 470));
    vehicles.push_back(std::make_unique<Vehicle>(6, Direction::North, Movement::Straight, 450, 390));
    vehicles.push_back(std::make_unique<Vehicle>(7, Direction::East,  Movement::Straight, 50,  250));
    vehicles.push_back(std::make_unique<Vehicle>(8, Direction::East,  Movement::Straight, 130, 250));
    vehicles.push_back(std::make_unique<Vehicle>(9, Direction::East,  Movement::Straight, 210, 250));
    vehicles.push_back(std::make_unique<Vehicle>(10, Direction::West, Movement::Straight, 750, 350));
    vehicles.push_back(std::make_unique<Vehicle>(11, Direction::West, Movement::Straight, 670, 350));
    vehicles.push_back(std::make_unique<Vehicle>(12, Direction::West, Movement::Straight, 590, 350));
    vehicles.push_back(std::make_unique<Vehicle>(13, Direction::South, Movement::Straight, 350, 290));

    auto nextTickTime = std::chrono::steady_clock::now();

    for (int tick = 0; tick <= 2000; tick++) {
        light.update(tick);
        
        for (auto& v : vehicles) {
            v->setDistanceToStopLine(calcDistToStopLine(v->getX(), v->getY(), v->getDirection()));
        }
        
        // Giả lập rule cho test
        for (auto& v : vehicles) {
            LightColor c = light.getColor(v->getDirection(), v->getMovement());
            if (v->getDistanceToStopLine() >= 0.0f) {
                if (c == LightColor::Red || c == LightColor::Yellow) {
                    v->setBrakeForLight(true); v->setCanProceed(false);
                } else {
                    v->setBrakeForLight(false); v->setCanProceed(true);
                }
            } else {
                v->setBrakeForLight(false); v->setCanProceed(true);
            }
        }

        for (auto& v : vehicles) v->update(0.02f);

        if (tick % 50 == 0) {
            std::cout << "===== Giay " << (tick / 50) << " =====\n";
            for (auto& v : vehicles) {
                std::cout << " Xe " << std::setw(2) << v->getId()
                          << " | Pos(" << std::setw(4) << (int)v->getX() << "," << std::setw(4) << (int)v->getY() << ")"
                          << " | Spd=" << std::fixed << std::setprecision(1) << v->getSpeed()
                          << " | DistLine=" << std::setw(5) << (int)v->getDistanceToStopLine()
                          << " | State=" << std::setw(13) << (v->getState() ? v->getState()->getName() : "?")
                          << " | Brake=" << (v->shouldBrake() ? "Y" : "N") << "\n";
            }
            std::cout << "\n";
        }

        nextTickTime += std::chrono::milliseconds(20);
        std::this_thread::sleep_until(nextTickTime);
    }
    return 0;
}