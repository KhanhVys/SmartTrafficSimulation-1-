#include "VehicleRenderer.h"

#include <cmath>

VehicleRenderer::VehicleRenderer(sf::Vector2f startPos, Direction dir, Movement mov) 
    : direction(dir), movement(mov) {
    
    // 1. Thân xe
    body.setSize(sf::Vector2f{50.0f, 26.0f});
    body.setFillColor(sf::Color(30, 144, 255));
    body.setOrigin(sf::Vector2f{25.0f, 13.0f});

    // 2. Mui xe / Kính
    roof.setSize(sf::Vector2f{22.0f, 20.0f});
    roof.setFillColor(sf::Color(180, 220, 240));
    roof.setOrigin(sf::Vector2f{11.0f, 10.0f});

    // 3. Đèn pha
    headlight.setSize(sf::Vector2f{4.0f, 20.0f});
    headlight.setFillColor(sf::Color(255, 255, 150));
    headlight.setOrigin(sf::Vector2f{2.0f, 10.0f});

    // 4. Bánh xe
    sf::Vector2f wheelSize{12.0f, 6.0f};
    sf::Color wheelColor = sf::Color(20, 20, 20);

    wheel1.setSize(wheelSize); wheel1.setFillColor(wheelColor); wheel1.setOrigin(sf::Vector2f{6.0f, 3.0f});
    wheel2.setSize(wheelSize); wheel2.setFillColor(wheelColor); wheel2.setOrigin(sf::Vector2f{6.0f, 3.0f});
    wheel3.setSize(wheelSize); wheel3.setFillColor(wheelColor); wheel3.setOrigin(sf::Vector2f{6.0f, 3.0f});
    wheel4.setSize(wheelSize); wheel4.setFillColor(wheelColor); wheel4.setOrigin(sf::Vector2f{6.0f, 3.0f});

    // 5. Khởi tạo đèn xi-nhan (màu cam)
    sf::Vector2f signalSize{4.0f, 4.0f};
    sf::Color orangeColor(255, 140, 0);

    leftSignal.setSize(signalSize);
    leftSignal.setFillColor(orangeColor);
    leftSignal.setOrigin(sf::Vector2f{2.0f, 2.0f});

    rightSignal.setSize(signalSize);
    rightSignal.setFillColor(orangeColor);
    rightSignal.setOrigin(sf::Vector2f{2.0f, 2.0f});

    setPosition(startPos);
}

void VehicleRenderer::setPosition(sf::Vector2f pos) {
    float rad = rotationDeg * 3.14159265f / 180.0f;
    float c = std::cos(rad);
    float s = std::sin(rad);

    // Đặt từng bộ phận theo vị trí tương đối so với tâm xe, xoay theo góc của xe
    auto place = [&](sf::RectangleShape& part, float ox, float oy) {
        part.setPosition(sf::Vector2f{pos.x + ox * c - oy * s, pos.y + ox * s + oy * c});
        part.setRotation(sf::degrees(rotationDeg));
    };

    place(body,      0.0f,   0.0f);
    place(roof,     -2.0f,   0.0f);
    place(headlight, 23.0f,  0.0f);

    place(wheel1,    14.0f, -14.0f);
    place(wheel2,    14.0f,  14.0f);
    place(wheel3,   -14.0f, -14.0f);
    place(wheel4,   -14.0f,  14.0f);

    // Vị trí 2 xi-nhan đặt ở 2 góc đầu xe phía trước
    place(leftSignal,  22.0f, -11.0f);
    place(rightSignal, 22.0f,  11.0f);
}

void VehicleRenderer::move(sf::Vector2f offset) {
    setPosition(sf::Vector2f{body.getPosition().x + offset.x, body.getPosition().y + offset.y});
}

void VehicleRenderer::setRotation(float degrees) {
    rotationDeg = degrees;
    setPosition(body.getPosition());
}

void VehicleRenderer::setTurnSignal(TurnSignal signal) {
    currentSignal = signal;
}

void VehicleRenderer::update() {
    if (blinkClock.getElapsedTime().asSeconds() > 0.25f) {
        blinkVisible = !blinkVisible;
        blinkClock.restart();
    }
}

void VehicleRenderer::draw(sf::RenderWindow& window) {
    window.draw(wheel1); window.draw(wheel2);
    window.draw(wheel3); window.draw(wheel4);
    window.draw(body); window.draw(roof); window.draw(headlight);

    if (blinkVisible) {
        if (currentSignal == TurnSignal::Left) {
            window.draw(leftSignal);
        }
        else if (currentSignal == TurnSignal::Right) {
            window.draw(rightSignal);
        }
    }
}

sf::Vector2f VehicleRenderer::getPosition() const {
    return body.getPosition();
}

float VehicleRenderer::getRotation() const {
    return rotationDeg;
}

Direction VehicleRenderer::getDirection() const {
    return direction;
}

Movement VehicleRenderer::getMovement() const {
    return movement;
}
