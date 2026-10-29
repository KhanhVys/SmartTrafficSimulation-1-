#pragma once

#include <SFML/Graphics.hpp>

enum class LightColor { Red, Yellow, Green };

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

public:
    TrafficLightRenderer(sf::Vector2f pos, bool leftTurn = false);
    void setState(LightColor state);
    void draw(sf::RenderWindow& window);
};