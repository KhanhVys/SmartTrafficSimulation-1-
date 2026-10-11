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
            const auto& light = engine.getTrafficLight();

            std::cout << "\033[2J\033[1H";

            std::cout << "=========================================================\n";
            std::cout << "     SMART URBAN TRAFFIC SIMULATION (LIVE DASHBOARD)     \n";
            std::cout << "=========================================================\n";
            std::cout << " [TIME: " << std::fixed << std::setprecision(1) << sec << "s / 38.0s] - Chu ky den giao thong:\n"
                      << "  - BN (Thang + Phai) : " << (light.getBNStraightRight() == SignalColor::Green ? "GREEN" : (light.getBNStraightRight() == SignalColor::Yellow ? "YELLOW" : "RED")) << "\n"
                      << "  - BN (Trai + Quay)  : " << (light.getBNLeftUTurn() == SignalColor::Green ? "GREEN" : (light.getBNLeftUTurn() == SignalColor::Yellow ? "YELLOW" : "RED")) << "\n"
                      << "  - DT (Thang + Phai) : " << (light.getDTStraightRight() == SignalColor::Green ? "GREEN" : (light.getDTStraightRight() == SignalColor::Yellow ? "YELLOW" : "RED")) << "\n"
                      << "  - DT (Trai + Quay)  : " << (light.getDTLeftUTurn() == SignalColor::Green ? "GREEN" : (light.getDTLeftUTurn() == SignalColor::Yellow ? "YELLOW" : "RED")) << "\n";
            std::cout << "---------------------------------------------------------\n";
            std::cout << std::left << std::setw(12) << "Vehicle ID"
                      << std::setw(10) << "Dir"
                      << std::setw(12) << "State"
                      << std::setw(10) << "Speed"
                      << std::setw(12) << "DistToStop\n";
            std::cout << "---------------------------------------------------------\n";

            for (const auto& v : engine.getAllVehicles()) {
                std::string dirStr = (v->getDirection() == Direction::North) ? "North" :
                                     (v->getDirection() == Direction::South) ? "South" :
                                     (v->getDirection() == Direction::East) ? "East" : "West";
                std::cout << std::left << std::setw(12) << v->getId()
                          << std::setw(10) << dirStr
                          << std::setw(12) << v->getStateName()
                          << std::setw(10) << std::setprecision(1) << v->getSpeed()
                          << std::setw(12) << std::setprecision(1) << v->getPosition() << "\n";
            }
            std::cout << "=========================================================\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\n=== MO PHONG HOAN TAT THANH CONG! ===\n";
    std::cout << "Nhan phim Enter de thoat...";
    std::cin.get();
    return 0;
}