#include "TrafficLightRenderer.h"

TrafficLightRenderer::TrafficLightRenderer(sf::Vector2f pos) {
    box.setSize(sf::Vector2f{24.0f, 66.0f});
    box.setFillColor(sf::Color(30, 30, 30));
    box.setPosition(pos);

    redLight.setRadius(7.0f); redLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 5.0f});
    yellowLight.setRadius(7.0f); yellowLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 25.0f});
    greenLight.setRadius(7.0f); greenLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 45.0f});

    setState(LightColorState::Red);
}

void TrafficLightRenderer::setState(LightColorState state) {
    sf::Color offColor(60, 60, 60);

    redLight.setFillColor(state == LightColorState::Red ? sf::Color::Red : offColor);
    yellowLight.setFillColor(state == LightColorState::Yellow ? sf::Color::Yellow : offColor);
    greenLight.setFillColor(state == LightColorState::Green ? sf::Color::Green : offColor);
}

void TrafficLightRenderer::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(redLight);
    window.draw(yellowLight);
    window.draw(greenLight);
}