#include "Vehicle.h"
#include "TrafficLight.h"
#include "States.h"
#include "Intersection.h" // Gọi file vừa tạo
#include "Road.h"         // Gọi file vừa tạo
#include "common/Constants.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <thread>   
#include <chrono>   
#include <algorithm> 

Vehicle* findVehicleAhead(Vehicle* cur, std::vector<std::unique_ptr<Vehicle>>& vs) {
    Vehicle* best = nullptr; float bestDist = 1e9f;
    for (auto& v : vs) {
        if (v.get() == cur) continue;
        if (v->getDirection() != cur->getDirection()) continue;
        float dx = v->getPosition().x - cur->getPosition().x;
        float dy = v->getPosition().y - cur->getPosition().y;
        float d = std::sqrt(dx*dx + dy*dy);
        bool ahead = false;
        switch (cur->getDirection()) {
            case Direction::South: ahead = v->getPosition().y > cur->getPosition().y; break;
            case Direction::North: ahead = v->getPosition().y < cur->getPosition().y; break;
            case Direction::East:  ahead = v->getPosition().x > cur->getPosition().x; break;
            case Direction::West:  ahead = v->getPosition().x < cur->getPosition().x; break;
        }
        if (ahead && d < bestDist) { bestDist = d; best = v.get(); }
    }
    return best;
}

int main() {
    TrafficLight light;
    
    // KHỞI TẠO COMPOSITE PATTERN
    auto intersection = std::make_shared<Intersection>();
    auto road1 = std::make_shared<Road>();
    auto road2 = std::make_shared<Road>();
    intersection->addRoad(road1);
    intersection->addRoad(road2);

    std::vector<std::unique_ptr<Vehicle>> vehicles;

    // Tạo đủ 13 xe
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

    auto nextTickTime = std::chrono::steady_clock::now();

    for (int tick = 0; tick <= 1500; tick++) {
        light.update(tick);
        
        for (auto& v : vehicles) {
            v->setVehicleAhead(findVehicleAhead(v.get(), vehicles));
            // Gọi qua Interface
            v->setDistanceToStopLine(intersection->getDistanceToStopLine(v->getPosition().x, v->getPosition().y, v->getDirection()));
        }
        
        for (auto& v : vehicles) {
            LightColor c = light.getColor(v->getDirection(), v->getMovement());
            v->setLightColor(c);

            if (!v->hasPassedStopLine()) {
                if (c == LightColor::Red || c == LightColor::Yellow) {
                    v->setBrakeForLight(true); v->setCanProceed(false);
                } else {
                    v->setBrakeForLight(false); v->setCanProceed(true);
                }
            } else {
                v->setBrakeForLight(false); v->setCanProceed(true);
            }
            
            if (v->getMovement() != Movement::Straight && std::abs(v->getDistanceToStopLine()) < 15.0f) {
                v->setShouldTurn(true);
            }
        }

        for (auto& v : vehicles) v->update(0.02f);

        vehicles.erase(std::remove_if(vehicles.begin(), vehicles.end(), 
            [](const std::unique_ptr<Vehicle>& v) { return v->isOutOfBounds(); }), vehicles.end());

        if (tick % 50 == 0) {
            std::cout << "===== Giay " << (tick / 50) << " =====\n";
            for (auto& v : vehicles) {
                auto pos = v->getPosition();
                std::cout << " Xe " << std::setw(2) << v->getId()
                          << " | Pos(" << std::setw(4) << (int)pos.x << "," << std::setw(4) << (int)pos.y << ")"
                          << " | Spd=" << std::fixed << std::setprecision(1) << v->getSpeed()
                          << " | Dist=" << std::setw(5) << (int)v->getDistanceToStopLine()
                          << " | State=" << std::setw(13) << (v->getState() ? v->getState()->getName() : "?")
                          << " | Mov=" << (v->getMovement() == Movement::Straight ? "Thang" : 
                                          v->getMovement() == Movement::Left ? "Trai" :
                                          v->getMovement() == Movement::Right ? "Phai" : "QuayDau")
                          << "\n";
            }
            std::cout << "\n";
        }

        nextTickTime += std::chrono::milliseconds(20);
        std::this_thread::sleep_until(nextTickTime);
    }
    return 0;
}