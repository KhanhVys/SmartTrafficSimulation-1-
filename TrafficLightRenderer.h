#pragma once

#include <SFML/Graphics.hpp>

enum class LightColorState { Red, Yellow, Green };

class TrafficLightRenderer {
private:
    sf::RectangleShape box;
    sf::CircleShape redLight;
    sf::CircleShape yellowLight;
    sf::CircleShape greenLight;

public:
    TrafficLightRenderer(sf::Vector2f pos);
    void setState(LightColorState state);
    void draw(sf::RenderWindow& window);
};