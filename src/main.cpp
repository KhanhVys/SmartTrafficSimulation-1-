#include "Vehicle.h"
#include "TrafficLight.h"
#include "States.h"
#include "Intersection.h" 
#include "Road.h"
#include "common/Constants.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <thread>   
#include <chrono>   
#include <algorithm> 

int main() {
    std::cout << "BAT DAU CHAY MO PHONG..." << std::endl;
    
    TrafficLight light;
    auto intersection = std::make_shared<Intersection>();
    intersection->addRoad(std::make_shared<Road>());

    std::vector<std::unique_ptr<Vehicle>> vehicles;
    
    // Tạo 13 xe
    vehicles.push_back(std::make_unique<Vehicle>(1, Direction::South, Movement::Straight, 350, 50, 0));
    vehicles.push_back(std::make_unique<Vehicle>(2, Direction::South, Movement::Straight, 350, 130, 0));
    vehicles.push_back(std::make_unique<Vehicle>(3, Direction::South, Movement::Left, 380, 210, 1));
    vehicles.push_back(std::make_unique<Vehicle>(4, Direction::North, Movement::Left, 450, 550, 1));
    vehicles.push_back(std::make_unique<Vehicle>(5, Direction::North, Movement::Right, 450, 470, 2));
    vehicles.push_back(std::make_unique<Vehicle>(6, Direction::North, Movement::Straight, 450, 390, 0));
    vehicles.push_back(std::make_unique<Vehicle>(7, Direction::East,  Movement::Straight, 50,  250, 1));
    vehicles.push_back(std::make_unique<Vehicle>(8, Direction::East,  Movement::Right, 130, 220, 2));
    vehicles.push_back(std::make_unique<Vehicle>(9, Direction::East,  Movement::Straight, 210, 250, 1));
    vehicles.push_back(std::make_unique<Vehicle>(10, Direction::West, Movement::Left, 750, 350, 1));
    vehicles.push_back(std::make_unique<Vehicle>(11, Direction::West, Movement::Straight, 670, 350, 1));
    vehicles.push_back(std::make_unique<Vehicle>(12, Direction::West, Movement::Straight, 590, 350, 1));
    vehicles.push_back(std::make_unique<Vehicle>(13, Direction::South, Movement::Straight, 350, 290, 0));

    std::cout << "DA KHOI TAO " << vehicles.size() << " XE." << std::endl;

    auto nextTickTime = std::chrono::steady_clock::now();

    for (int tick = 0; tick <= 1500; tick++) {
        light.update(tick);
        
        for (auto& v : vehicles) {
            v->checkVehicleAhead(vehicles);
            v->setDistanceToStopLine(intersection->getDistanceToStopLine(v->getPosition().x, v->getPosition().y, v->getDirection()));
        }
        
        for (auto& v : vehicles) {
            LightColor c = light.getColor(v->getDirection(), v->getMovement());
            v->setLightColor(c);
            if (!v->hasPassedStopLine()) {
                v->setBrakeForLight(c == LightColor::Red || c == LightColor::Yellow);
                v->setCanProceed(c == LightColor::Green);
            } else {
                v->setBrakeForLight(false); v->setCanProceed(true);
            }
            if (v->getMovement() != Movement::Straight && v->getDistanceToStopLine() <= 0.0f && !v->getHasTurned()) {
                v->setShouldTurn(true);
            }
        }

        for (auto& v : vehicles) v->update(0.02f);

        vehicles.erase(std::remove_if(vehicles.begin(), vehicles.end(), 
            [](const std::unique_ptr<Vehicle>& v) { return v->isOutOfBounds(); }), vehicles.end());

        if (tick % 50 == 0) {
            std::cout << "===== Giay " << (tick / 50) << " =====" << std::endl;
            for (auto& v : vehicles) {
                auto pos = v->getPosition();
                std::cout << " Xe " << std::setw(2) << v->getId()
                          << " | Pos(" << std::setw(4) << (int)pos.x << "," << std::setw(4) << (int)pos.y << ")"
                          << " | Spd=" << std::fixed << std::setprecision(1) << v->getSpeed()
                          << " | Dist=" << std::setw(5) << (int)v->getDistanceToStopLine()
                          << std::endl;
            }
            std::cout << std::endl;
        }

        nextTickTime += std::chrono::milliseconds(20);
        std::this_thread::sleep_until(nextTickTime);
    }
    
    std::cout << "KET THUC MO PHONG." << std::endl;
    return 0;
}