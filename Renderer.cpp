#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cstddef>

#include "MapRenderer.h"
#include "VehicleRenderer.h"
#include "TrafficLightRenderer.h"

// Trạng thái chuyển động của từng xe (chỉ phục vụ hiển thị)
struct CarMotion {
    int lane = 0;       // 0 = làn trong (sát dải phân cách), 1 = làn giữa, 2 = làn ngoài
    int phase = 0;      // 0 = đang tiến tới, 1 = đang rẽ (cung tròn), 2 = đã rẽ xong, chạy ra khỏi màn hình
    float arc = 0.0f;   // quãng đường đã đi trên cung tròn (px)
};

class Renderer {
private:
    sf::RenderWindow window;
    MapRenderer mapRenderer;
    std::vector<VehicleRenderer> vehicles;
    std::vector<CarMotion> motions;

    std::vector<TrafficLightRenderer> lightsNSStraight;  // Bắc-Nam: thẳng
    std::vector<TrafficLightRenderer> lightsNSLeft;      // Bắc-Nam: trái/quay đầu
    std::vector<TrafficLightRenderer> lightsEWStraight;  // Đông-Tây: thẳng
    std::vector<TrafficLightRenderer> lightsEWLeft;      // Đông-Tây: trái/quay đầu

    std::size_t phaseIndex = 0;
    sf::Clock cycleClock;

    void resetVehicles();

public:
    Renderer(unsigned int width, unsigned int height, const std::string& title);

    bool isWindowOpen() const;
    void handleEvents();
    void update();
    void render(const SimulationSnapshot&) ;
};
