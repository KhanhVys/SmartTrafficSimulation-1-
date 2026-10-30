#include "TrafficLightRenderer.h"

TrafficLightRenderer::TrafficLightRenderer(sf::Vector2f pos, bool leftTurn, float arrowAngleDeg)
    : hasArrow(leftTurn) {

    if (!leftTurn) {
        // Normal 3-lamp light (straight + right)
        box.setSize(sf::Vector2f{24.0f, 66.0f});
        box.setFillColor(sf::Color(30, 30, 30));
        box.setPosition(pos);

        redLight.setRadius(7.0f);    redLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 5.0f});
        yellowLight.setRadius(7.0f); yellowLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 25.0f});
        greenLight.setRadius(7.0f);  greenLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 45.0f});
    } else {
        // Left-turn light: 28x28 box, centered vertically next to the 66px light
        float ox = pos.x;
        float oy = pos.y + 19.0f;

        box.setSize(sf::Vector2f{28.0f, 28.0f});
        box.setFillColor(sf::Color(30, 30, 30));
        box.setPosition(sf::Vector2f{ox, oy});

        arrowLamp.setRadius(11.0f);
        arrowLamp.setFillColor(sf::Color(15, 15, 15));
        arrowLamp.setPosition(sf::Vector2f{ox + 3.0f, oy + 3.0f});

        // Tâm đèn: mũi tên xoay quanh điểm này
        sf::Vector2f center{ox + 14.0f, oy + 14.0f};

        // Thân mũi tên (gốc xoay ở đầu thân, đặt đúng tâm đèn)
        arrowShaft.setSize(sf::Vector2f{9.0f, 4.0f});
        arrowShaft.setOrigin(sf::Vector2f{0.0f, 2.0f});
        arrowShaft.setPosition(center);
        arrowShaft.setRotation(sf::degrees(arrowAngleDeg));

        // Đầu mũi tên (gốc xoay đặt đúng tâm đèn)
        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f{0.0f, 7.0f});    // tip
        arrow.setPoint(1, sf::Vector2f{8.0f, 0.0f});
        arrow.setPoint(2, sf::Vector2f{8.0f, 14.0f});
        arrow.setOrigin(sf::Vector2f{8.0f, 7.0f});
        arrow.setPosition(center);
        arrow.setRotation(sf::degrees(arrowAngleDeg));
    }

    setState(LightColor::Red);
}

void TrafficLightRenderer::setState(LightColor state) {
    if (hasArrow) {
        sf::Color c = sf::Color::Red;
        if (state == LightColor::Green)       c = sf::Color::Green;
        else if (state == LightColor::Yellow) c = sf::Color::Yellow;
        arrowShaft.setFillColor(c);
        arrow.setFillColor(c);
        return;
    }

    sf::Color offColor(60, 60, 60);

    redLight.setFillColor(state == LightColor::Red ? sf::Color::Red : offColor);
    yellowLight.setFillColor(state == LightColor::Yellow ? sf::Color::Yellow : offColor);
    greenLight.setFillColor(state == LightColor::Green ? sf::Color::Green : offColor);
}

void TrafficLightRenderer::draw(sf::RenderWindow& window) {
    window.draw(box);

    if (hasArrow) {
        window.draw(arrowLamp);
        window.draw(arrowShaft);
        window.draw(arrow);
        return;
    }

    window.draw(redLight);
    window.draw(yellowLight);
    window.draw(greenLight);
}