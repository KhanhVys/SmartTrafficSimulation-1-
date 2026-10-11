#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "Vehicle.h"
#include "TrafficLight.h"

int main() {
  
    std::system("chcp 65001 > nul");

    std::cout << "================================================================================\n";
    std::cout << "     HE THONG MO PHONG GIAO THONG -(CORE SIMULATION)\n";
    std::cout << "     Trang thai Xe & Chu ky Den Giao Thong 38 Giay\n";
    std::cout << "================================================================================\n\n";

    Vehicle carBN("CAR_BacNam_01", 10.0, 13.89);
    Vehicle carDT("CAR_DongTay_01", 10.0, 13.89); 
    TrafficLight light;

    const double dt = 0.02;        
    const int totalTicks = 1900;   
    std::cout << std::left 
              << std::setw(10) << "TICK" 
              << std::setw(10) << "TIME(s)" 
              << std::setw(32) << "PHA DEN GIAO THONG" 
              << std::setw(18) << "TRANG THAI XE BN" 
              << std::setw(15) << "TOA DO (m)" 
              << std::setw(15) << "TOC DO (km/h)" << "\n";
    std::cout << "----------------------------------------------------------------------------------------------------\n";

    for (int tick = 0; tick <= totalTicks; ++tick) {
        light.update(dt);
        
        double distBN = 200.0 - carBN.getPosition();
        double distDT = 200.0 - carDT.getPosition();

        carBN.update(distBN, light.isNorthSouthRed(), dt);
        carDT.update(distDT, light.isEastWestRed(), dt);

        if (tick % 25 == 0) {
            std::cout << std::left 
                      << std::setw(10) << tick
                      << std::setw(10) << std::fixed << std::setprecision(2) << light.getCycleTimer()
                      << std::setw(32) << light.getCurrentPhaseName()
                      << std::setw(18) << carBN.getStateName()
                      << std::setw(15) << std::fixed << std::setprecision(1) << carBN.getPosition()
                      << std::setw(15) << std::fixed << std::setprecision(1) << carBN.getSpeedKmh() << "\n";
        }

       
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "\n================================================================================\n";
    std::cout << "[SUCCESS] Hoan tat toan bo chu ky mo phong Nhiem vu Nguoi 1!\n";
    std::cout << "Nhan phim [Enter] de thoat chuong trinh...";
    std::cin.get(); 

    return 0;
}