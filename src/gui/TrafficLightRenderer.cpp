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

        // Arrow pointing left: shaft + head
        arrowShaft.setSize(sf::Vector2f{9.0f, 4.0f});
        arrowShaft.setPosition(sf::Vector2f{ox + 14.0f, oy + 12.0f});

        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f{0.0f, 7.0f});    // tip
        arrow.setPoint(1, sf::Vector2f{8.0f, 0.0f});
        arrow.setPoint(2, sf::Vector2f{8.0f, 14.0f});
        arrow.setPosition(sf::Vector2f{ox + 6.0f, oy + 7.0f});
    }

    setState(LightColor::Red);
}

void TrafficLightRenderer::setState(LightColor state) {
    sf::Color activeColor = sf::Color::Red;

    if (state == LightColor::Green) {
        activeColor = sf::Color::Green;
    }
    else if (state == LightColor::Yellow) {
        activeColor = sf::Color::Yellow;
    }

    glowColor = activeColor;
    glowOn = true;

    if (hasArrow) {
        arrowLamp.setFillColor(sf::Color(15, 15, 15));
        arrowShaft.setFillColor(activeColor);
        arrow.setFillColor(activeColor);

        glowCenter = sf::Vector2f{
            arrowLamp.getPosition().x + 11.0f,
            arrowLamp.getPosition().y + 11.0f
        };
        return;
    }

    sf::Color offColor(60, 60, 60);

    redLight.setFillColor(state == LightColor::Red ? sf::Color::Red : offColor);
    yellowLight.setFillColor(state == LightColor::Yellow ? sf::Color::Yellow : offColor);
    greenLight.setFillColor(state == LightColor::Green ? sf::Color::Green : offColor);

    if (state == LightColor::Red) {
        glowCenter = sf::Vector2f{
            redLight.getPosition().x + 7.0f,
            redLight.getPosition().y + 7.0f
        };
    }
    else if (state == LightColor::Yellow) {
        glowCenter = sf::Vector2f{
            yellowLight.getPosition().x + 7.0f,
            yellowLight.getPosition().y + 7.0f
        };
    }
    else {
        glowCenter = sf::Vector2f{
            greenLight.getPosition().x + 7.0f,
            greenLight.getPosition().y + 7.0f
        };
    }
}

void TrafficLightRenderer::draw(sf::RenderWindow& window) {
    window.draw(box);

    // Hiệu ứng quầng sáng
    if (glowOn) {
        sf::CircleShape outerGlow(17.0f);
        outerGlow.setOrigin(sf::Vector2f{17.0f, 17.0f});
        outerGlow.setPosition(glowCenter);

        sf::Color outerColor = glowColor;
        outerColor.a = 30;
        outerGlow.setFillColor(outerColor);
        window.draw(outerGlow);

        sf::CircleShape innerGlow(12.0f);
        innerGlow.setOrigin(sf::Vector2f{12.0f, 12.0f});
        innerGlow.setPosition(glowCenter);

        sf::Color innerColor = glowColor;
        innerColor.a = 55;
        innerGlow.setFillColor(innerColor);
        window.draw(innerGlow);
    }

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