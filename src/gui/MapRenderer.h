#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class MapRenderer {
private:
    sf::RectangleShape northSouthRoad;  // Đường đơn Bắc - Nam (2 làn rộng)
    sf::RectangleShape eastWestRoad;    // Đường đôi Đông - Tây (6 làn)
    sf::RectangleShape medianStrip;     // Dải phân cách trung tâm Đ-T
    std::vector<sf::RectangleShape> laneLines; // Vạch kẻ đường
    std::vector<sf::RectangleShape> stopLines;  // Vạch dừng xe trước ngã tư
    std::vector<sf::RectangleShape> crosswalks; // Vạch qua đường (ngựa vằn)
    std::vector<sf::ConvexShape> cornerBlocks;  // Vỉa hè + bó vỉa + viền trắng quanh 4 góc
public:
    MapRenderer(float width, float height);
    void draw(sf::RenderWindow& window);
};