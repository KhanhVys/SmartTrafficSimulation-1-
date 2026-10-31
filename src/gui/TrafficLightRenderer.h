#pragma once

#include <SFML/Graphics.hpp>
#include "common/Types.h"

class TrafficLightRenderer {
private:
    sf::RectangleShape box;
    sf::CircleShape redLight;
    sf::CircleShape yellowLight;
    sf::CircleShape greenLight;

    // Left-turn light
    sf::CircleShape arrowLamp;
    sf::RectangleShape arrowShaft;
    sf::ConvexShape arrow;
    bool hasArrow;

    // Glow effect
    sf::Vector2f glowCenter{0.0f, 0.0f};
    sf::Color glowColor{0, 0, 0, 0};
    bool glowOn = false;

public:
    TrafficLightRenderer(
        sf::Vector2f pos,
        bool leftTurn = false,
        float arrowAngleDeg = 0.0f
    );

    void setState(LightColor state);
    void draw(sf::RenderWindow& window);
};