#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "SimulationEngine.h"

int main() {
    SimulationEngine engine;
    const double dt = 0.02; 
    int totalTicks = 1900;  

    for (int tick = 0; tick <= totalTicks; ++tick) {
        engine.step(dt);

        if (tick % 50 == 0) {
            double sec = engine.getTime();
            const auto& light = engine.getIntersection().getTrafficLight();

            std::cout << "\033[2J\033[1H";
            std::cout << "=========================================================\n";
            std::cout << "      SMART TRAFFIC SIMULATION - CORE MODULE (UPDATE)    \n";
            std::cout << "=========================================================\n";
            std::cout << " [TIME: " << std::fixed << std::setprecision(1) << sec << "s / 38.0s] - Trang thai den:\n"
                      << "  - BN (Thang/Phai) : " << (light.getBnStraightRightColor() == LightColor::Green ? "GREEN" : (light.getBnStraightRightColor() == LightColor::Yellow ? "YELLOW" : "RED")) << "\n"
                      << "  - BN (Trai/Quay)  : " << (light.getBnLeftUTurnColor() == LightColor::Green ? "GREEN" : "RED") << "\n"
                      << "  - DT (Thang/Phai) : " << (light.getDtStraightRightColor() == LightColor::Green ? "GREEN" : (light.getDtStraightRightColor() == LightColor::Yellow ? "YELLOW" : "RED")) << "\n"
                      << "  - DT (Trai/Quay)  : " << (light.getDtLeftUTurnColor() == LightColor::Green ? "GREEN" : "RED") << "\n";
            std::cout << "---------------------------------------------------------\n";
            std::cout << std::left << std::setw(14) << "Vehicle ID"
                      << std::setw(10) << "Dir"
                      << std::setw(14) << "State"
                      << std::setw(10) << "Speed"
                      << std::setw(12) << "DistToStop"
                      << std::setw(15) << "Coordinates (x, y)\n";
            std::cout << "---------------------------------------------------------\n";

            for (const auto& v : engine.getAllVehicles()) {
                std::string dirStr = (v->getDirection() == Direction::North) ? "North" :
                                     (v->getDirection() == Direction::South) ? "South" :
                                     (v->getDirection() == Direction::East) ? "East" : "West";
                std::cout << std::left << std::setw(14) << v->getVehicleId()
                          << std::setw(10) << dirStr
                          << std::setw(14) << v->getStateName()
                          << std::setw(10) << std::setprecision(1) << v->getSpeed()
                          << std::setw(12) << std::setprecision(1) << v->getPosition()
                          << "(" << std::fixed << std::setprecision(1) << v->getX() << ", " << v->getY() << ")\n";
            }
            std::cout << "=========================================================\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\n=== MO PHONG HOAN TAT THANH CONG! ===\n";
    return 0;
}
