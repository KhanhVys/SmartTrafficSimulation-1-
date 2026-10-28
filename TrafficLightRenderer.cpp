#include "TrafficLightRenderer.h"

TrafficLightRenderer::TrafficLightRenderer(sf::Vector2f pos, bool leftTurn)
    : hasArrow(leftTurn) {

    if (!leftTurn) {
        // Đèn thẳng: 3 bóng đỏ, vàng, xanh
        box.setSize(sf::Vector2f{24.0f, 66.0f});
        box.setFillColor(sf::Color(30, 30, 30));
        box.setPosition(pos);

        redLight.setRadius(7.0f);    redLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 5.0f});
        yellowLight.setRadius(7.0f); yellowLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 25.0f});
        greenLight.setRadius(7.0f);  greenLight.setPosition(sf::Vector2f{pos.x + 5.0f, pos.y + 45.0f});
    } else {
        // Đèn mũi tên trái: hộp 28x28, nằm giữa chiều cao của đèn thẳng
        float ox = pos.x;
        float oy = pos.y + 19.0f;

        box.setSize(sf::Vector2f{28.0f, 28.0f});
        box.setFillColor(sf::Color(30, 30, 30));
        box.setPosition(sf::Vector2f{ox, oy});

        arrowLamp.setRadius(11.0f);
        arrowLamp.setFillColor(sf::Color(15, 15, 15));
        arrowLamp.setPosition(sf::Vector2f{ox + 3.0f, oy + 3.0f});

        // Mũi tên chỉ sang trái: thân + đầu
        arrowShaft.setSize(sf::Vector2f{9.0f, 4.0f});
        arrowShaft.setPosition(sf::Vector2f{ox + 14.0f, oy + 12.0f});

        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f{0.0f, 7.0f});    // mũi
        arrow.setPoint(1, sf::Vector2f{8.0f, 0.0f});
        arrow.setPoint(2, sf::Vector2f{8.0f, 14.0f});
        arrow.setPosition(sf::Vector2f{ox + 6.0f, oy + 7.0f});
    }

    setState(LightColor::Red);
}

void TrafficLightRenderer::setState(LightColor state) {
    if (hasArrow) {
        // Mũi tên đổi màu theo đèn: đỏ, vàng hoặc xanh
        sf::Color c = sf::Color::Red;
        if (state == LightColor::Green)  c = sf::Color::Green;
        if (state == LightColor::Yellow) c = sf::Color::Yellow;

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
