#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class MapRenderer {
private:
    sf::RectangleShape northSouthRoad;  // Đường đơn Bắc - Nam (2 làn rộng)
    sf::RectangleShape eastWestRoad;    // Đường đôi Đông - Tây (6 làn)
    sf::RectangleShape medianStrip;     // Dải phân cách trung tâm Đ-T
    std::vector<sf::RectangleShape> laneLines; // Vạch kẻ đường

public:
    MapRenderer(float width, float height);
    void draw(sf::RenderWindow& window);
};