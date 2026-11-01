#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
class MapRenderer {
private:
    sf::RectangleShape northSouthRoad;  // Đường đơn Bắc - Nam (2 làn rộng)
    sf::RectangleShape eastWestRoad;    // Đường đôi Đông - Tây (6 làn)
    sf::RectangleShape medianLeft;      // Dải phân cách Đ-T, đoạn bên Tây ngã tư
    sf::RectangleShape medianRight;     // Dải phân cách Đ-T, đoạn bên Đông ngã tư
    std::vector<sf::RectangleShape> roadSpecks;
    std::vector<sf::RectangleShape> laneLines; // Vạch kẻ đường
    std::vector<sf::RectangleShape> stopLines;  // Vạch dừng xe trước ngã tư
    std::vector<sf::RectangleShape> crosswalks; // Vạch qua đường (ngựa vằn)
    std::vector<sf::ConvexShape> cornerBlocks;  // Vỉa hè + bó vỉa + viền trắng quanh 4 góc
    std::vector<sf::RectangleShape> decorRects;   // nhà, bóng đổ, thảm cỏ, sân lát
    std::vector<sf::CircleShape> decorCircles;    // tán cây, bụi cây, bồn cây
    
    public:
    MapRenderer(float width, float height);
    void draw(sf::RenderWindow& window);
};