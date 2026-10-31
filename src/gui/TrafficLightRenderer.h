#pragma once

#include <SFML/Graphics.hpp>
#include "common/Types.h"   // LightColor dùng chung toàn dự án

class TrafficLightRenderer {
private:
    sf::RectangleShape box;
    sf::CircleShape redLight;
    sf::CircleShape yellowLight;
    sf::CircleShape greenLight;

    // Left-turn light: one lamp with an arrow inside
    sf::CircleShape arrowLamp;
    sf::RectangleShape arrowShaft;
    sf::ConvexShape arrow;   // arrow head
    bool hasArrow;
    sf::Vector2f glowCenter {0.0f, 0.0f};
    sf::Color glowColor {0, 0, 0, 0};
    bool glowOn = false;

public:
    // approach: nhánh mà xe đi VÀO ngã tư (cùng nghĩa với Direction của Core).
    // Với đèn rẽ trái, mũi tên quay về phía bên trái của tài xế khi xe đi vào từ nhánh này.
    TrafficLightRenderer(sf::Vector2f pos, bool leftTurn = false,
                         Direction approach = Direction::South);
    void setState(LightColor state);
    void draw(sf::RenderWindow& window);
};