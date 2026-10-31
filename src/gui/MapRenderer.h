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
    std::vector<sf::RectangleShape> sidewalks; // Vỉa hè dọc mép đường
    std::vector<sf::RectangleShape> edgeLines; // Vạch liền ở mép đường
    std::vector<sf::CircleShape> trees;        // Tán cây
public:
    MapRenderer(float width, float height);
    void draw(sf::RenderWindow& window);
};