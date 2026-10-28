#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "common/Types.h"                    // LightColor, Direction, Movement
#include "concurrency/SimulationSnapshot.h"  // SimulationSnapshot của Người 3
#include "MapRenderer.h"
#include "VehicleRenderer.h"
#include "TrafficLightRenderer.h"

class Renderer {
private:
    // Mỗi nhánh (Bắc/Nam/Đông/Tây) có 2 đèn: đèn thẳng và mũi tên trái
    struct LightView {
        Direction approach;
        bool turnArrow;
        TrafficLightRenderer view;
    };

    sf::RenderWindow window;
    MapRenderer mapRenderer;
    std::vector<VehicleRenderer> vehicles;   // pool xe, tự tăng theo snapshot
    std::vector<LightView> lights;

    void addLight(Direction approach, bool turnArrow, sf::Vector2f pos);

public:
    Renderer(unsigned int width, unsigned int height, const std::string& title);

    bool isWindowOpen() const;
    void handleEvents();
    void render(const SimulationSnapshot& snapshot);
};
