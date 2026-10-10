#include "VehicleRenderer.h"

VehicleRenderer::VehicleRenderer(sf::Vector2f startPos) {
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

    // 5. BỔ SUNG KHỞI TẠO ĐÈN XI-NHAN (MÀU CAM)
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
    body.setPosition(pos);
    roof.setPosition(sf::Vector2f{pos.x - 2.0f, pos.y});
    headlight.setPosition(sf::Vector2f{pos.x + 23.0f, pos.y});

    wheel1.setPosition(sf::Vector2f{pos.x + 14.0f, pos.y - 14.0f});
    wheel2.setPosition(sf::Vector2f{pos.x + 14.0f, pos.y + 14.0f});
    wheel3.setPosition(sf::Vector2f{pos.x - 14.0f, pos.y - 14.0f});
    wheel4.setPosition(sf::Vector2f{pos.x - 14.0f, pos.y + 14.0f});

    // Vị trí 2 xi-nhan đặt ở 2 góc đầu xe phía trước
    leftSignal.setPosition(sf::Vector2f{pos.x + 22.0f, pos.y - 11.0f});
    rightSignal.setPosition(sf::Vector2f{pos.x + 22.0f, pos.y + 11.0f});
}

void VehicleRenderer::move(sf::Vector2f offset) {
    setPosition(sf::Vector2f{body.getPosition().x + offset.x, body.getPosition().y + offset.y});
}

// Hàm chuyển đổi bật xi-nhan
void VehicleRenderer::setTurnSignal(TurnSignal signal) {
    currentSignal = signal;
}

// Cập nhật nhấp nháy đèn mỗi 0.25 giây
void VehicleRenderer::update() {
    if (blinkClock.getElapsedTime().asSeconds() > 0.25f) {
        blinkVisible = !blinkVisible;
        blinkClock.restart();
    }
}

void VehicleRenderer::draw(sf::RenderWindow& window) {
    // Vẽ bánh xe, thân xe, kính và đèn pha
    window.draw(wheel1); window.draw(wheel2);
    window.draw(wheel3); window.draw(wheel4);
    window.draw(body); window.draw(roof); window.draw(headlight);

    // BỔ SUNG VẼ XI-NHAN KHI ĐƯỢC BẬT VÀ TRONG PHA SÁNG
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