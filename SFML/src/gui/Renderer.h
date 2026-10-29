#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/Types.h"                    
#include "concurrency/SimulationSnapshot.h"  
#include "MapRenderer.h"
#include "VehicleRenderer.h"
#include "TrafficLightRenderer.h"

class Renderer {
private:
    sf::RenderWindow window;
    MapRenderer mapRenderer;

    // Quản lý pool xe theo vehicleId (sử dụng emplace do VehicleRenderer không có constructor mặc định)
    std::unordered_map<int, VehicleRenderer> activeVehicles;

    // Cấu trúc ánh xạ đèn giao thông gắn liền với (direction, turnArrow)
    struct LightMapping {
        Direction direction;
        bool turnArrow;
        TrafficLightRenderer renderer;
    };
    std::vector<LightMapping> trafficLights;

    void updateLights(const std::vector<LightSnapshot>& lightSnapshots);
    void updateVehicles(const std::vector<VehicleSnapshot>& vehicleSnapshots);

public:
    Renderer(unsigned int width, unsigned int height, const std::string& title);

    bool isWindowOpen() const;
    void handleEvents();
    void render(const SimulationSnapshot& snapshot);
};