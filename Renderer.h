#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "MapRenderer.h"
#include "VehicleRenderer.h"
#include "TrafficLightRenderer.h"

class Renderer {
private:
    sf::RenderWindow window;
    MapRenderer mapRenderer;
    std::vector<VehicleRenderer> vehicles;
    std::vector<TrafficLightRenderer> trafficLights;

public:
    Renderer(unsigned int width, unsigned int height, const std::string& title);
    bool isWindowOpen() const;
    void handleEvents();
    void update();
    void render();
};