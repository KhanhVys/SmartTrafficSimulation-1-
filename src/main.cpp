#include "Vehicle.h"
#include "TrafficLight.h"
#include "States.h"
#include "common/Constants.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <thread>   
#include <chrono>   

// Hàm giả lập Logic tính khoảng cách tới vạch (của Người 4 / Road)
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

    // Khởi tạo 13 xe (Constructor 6 tham số: id, dir, movement, x, y, laneIndex)
    // Xe 1-3: Đi thẳng hướng Nam (test đèn đỏ)
    vehicles.push_back(std::make_unique<Vehicle>(1, Direction::South, Movement::Straight, 350, 50, 0));
    vehicles.push_back(std::make_unique<Vehicle>(2, Direction::South, Movement::Straight, 350, 130, 0));
    vehicles.push_back(std::make_unique<Vehicle>(3, Direction::South, Movement::Straight, 350, 210, 0));
    
    // Xe 4-5: Rẽ trái và rẽ phải hướng Bắc (test logic rẽ)
    vehicles.push_back(std::make_unique<Vehicle>(4, Direction::North, Movement::Left, 450, 550, 1));
    vehicles.push_back(std::make_unique<Vehicle>(5, Direction::North, Movement::Right, 450, 470, 2));
    
    // Xe 6: Quay đầu hướng Đông
    vehicles.push_back(std::make_unique<Vehicle>(6, Direction::East, Movement::UTurn, 50, 250, 1));

    // Các xe còn lại đi thẳng
    vehicles.push_back(std::make_unique<Vehicle>(7, Direction::East,  Movement::Straight, 130, 250, 0));
    vehicles.push_back(std::make_unique<Vehicle>(8, Direction::West, Movement::Straight, 750, 350, 0));

    auto nextTickTime = std::chrono::steady_clock::now();

    // Chạy mô phỏng 20 giây
    for (int tick = 0; tick <= 1000; tick++) {
        light.update(tick);
        
        // 1. Cập nhật vị trí cho xe (Giả lập Logic của Road/Engine)
        for (auto& v : vehicles) {
            v->setDistanceToStopLine(calcDistToStopLine(v->getPosition().x, v->getPosition().y, v->getDirection()));
        }
        
        // 2. Áp dụng luật giao thông (Giả lập TrafficRule của Người 2)
        for (auto& v : vehicles) {
            LightColor c = light.getColor(v->getDirection(), v->getMovement());
            
            if (!v->hasPassedStopLine()) {
                if (c == LightColor::Red || c == LightColor::Yellow) {
                    v->setBrakeForLight(true); 
                    v->setCanProceed(false);
                } else {
                    v->setBrakeForLight(false); 
                    v->setCanProceed(true);
                }
            } else {
                v->setBrakeForLight(false); 
                v->setCanProceed(true);
            }
        }

        // 3. Update xe
        for (auto& v : vehicles) {
            v->update(0.02f);
        }

        // In Log mỗi giây
        if (tick % 50 == 0) {
            std::cout << "===== Giay " << (tick / 50) << " =====\n";
            for (auto& v : vehicles) {
                auto pos = v->getPosition();
                std::cout << " Xe " << std::setw(2) << v->getId()
                          << " | Pos(" << std::setw(4) << (int)pos.x << "," << std::setw(4) << (int)pos.y << ")"
                          << " | Spd=" << std::fixed << std::setprecision(1) << v->getSpeed()
                          << " | DistLine=" << std::setw(5) << (int)v->getDistanceToStopLine()
                          << " | State=" << std::setw(13) << (v->getState() ? v->getState()->getName() : "?")
                          << " | Mov=" << (v->getMovement() == Movement::Straight ? "Thang" : 
                                          v->getMovement() == Movement::Left ? "Trai" :
                                          v->getMovement() == Movement::Right ? "Phai" : "QuayDau")
                          << "\n";
            }
            std::cout << "\n";
        }

        // Giới hạn tốc độ chạy (20ms = 0.02s)
        nextTickTime += std::chrono::milliseconds(20);
        std::this_thread::sleep_until(nextTickTime);
    }
    return 0;
}