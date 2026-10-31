#include "VehicleRenderer.h"

#include <cmath>
#include <cstdint>
namespace {
    // Kích thước chuẩn theo yêu cầu của Core (CAR_LENGTH = 40, CAR_WIDTH = 20)
    constexpr float CAR_LENGTH = 40.0f;
    constexpr float CAR_WIDTH  = 20.0f;
    constexpr float HALF_L     = CAR_LENGTH / 2.0f; // 20.0f
    constexpr float HALF_W     = CAR_WIDTH / 2.0f;  // 10.0f
}
sf::Color VehicleRenderer::getVehicleColor(int vehicleId) {
    const sf::Color colors[] = {
        sf::Color(30, 144, 255),
        sf::Color(220, 60, 60),
        sf::Color(255, 190, 60),
        sf::Color(80, 180, 120),
        sf::Color(170, 110, 210),
        sf::Color(240, 130, 160)
    };

    return colors[vehicleId % 6];
}
VehicleRenderer::VehicleRenderer(sf::Vector2f startPos, Direction dir, Movement mov, int vehicleId)
    : direction(dir), movement(mov) {
    
        // 1. Thân xe (40x20) + viền tối hơn màu thân
    const sf::Color bodyColor = getVehicleColor(vehicleId);
    body.setSize(sf::Vector2f{CAR_LENGTH, CAR_WIDTH});
    body.setFillColor(bodyColor);
    body.setOrigin(sf::Vector2f{HALF_L, HALF_W});
    body.setOutlineThickness(-1.5f);                     // âm: viền nằm bên trong, không đổi kích thước xe
    body.setOutlineColor(sf::Color(
        static_cast<std::uint8_t>(bodyColor.r * 0.55f),
        static_cast<std::uint8_t>(bodyColor.g * 0.55f),
        static_cast<std::uint8_t>(bodyColor.b * 0.55f)));

    // 2. Mui xe (sáng hơn thân một chút), kính trước, kính sau
    roof.setSize(sf::Vector2f{10.0f, 14.0f});
    roof.setFillColor(sf::Color(
        static_cast<std::uint8_t>(bodyColor.r + (255 - bodyColor.r) * 0.25f),
        static_cast<std::uint8_t>(bodyColor.g + (255 - bodyColor.g) * 0.25f),
        static_cast<std::uint8_t>(bodyColor.b + (255 - bodyColor.b) * 0.25f)));
    roof.setOrigin(sf::Vector2f{5.0f, 7.0f});

    windshield.setSize(sf::Vector2f{5.0f, 14.0f});
    windshield.setFillColor(sf::Color(175, 220, 240));
    windshield.setOrigin(sf::Vector2f{2.5f, 7.0f});

    rearWindow.setSize(sf::Vector2f{4.0f, 14.0f});
    rearWindow.setFillColor(sf::Color(125, 165, 190));
    rearWindow.setOrigin(sf::Vector2f{2.0f, 7.0f});

    // 3. Hai đèn pha ở đầu xe, hai đèn hậu ở đuôi xe
    sf::Vector2f headSize{3.0f, 4.0f};
    headlight.setSize(headSize);   headlight.setFillColor(sf::Color(255, 255, 170));  headlight.setOrigin(sf::Vector2f{1.5f, 2.0f});
    headlight2.setSize(headSize);  headlight2.setFillColor(sf::Color(255, 255, 170)); headlight2.setOrigin(sf::Vector2f{1.5f, 2.0f});

    sf::Vector2f tailSize{2.0f, 4.0f};
    taillight1.setSize(tailSize);  taillight1.setFillColor(sf::Color(200, 20, 20));   taillight1.setOrigin(sf::Vector2f{1.0f, 2.0f});
    taillight2.setSize(tailSize);  taillight2.setFillColor(sf::Color(200, 20, 20));   taillight2.setOrigin(sf::Vector2f{1.0f, 2.0f});

    // 3b. Bóng đổ: cùng kích thước thân xe, đen mờ
    shadow.setSize(sf::Vector2f{CAR_LENGTH, CAR_WIDTH});
    shadow.setFillColor(sf::Color(0, 0, 0, 70));
    shadow.setOrigin(sf::Vector2f{HALF_L, HALF_W});

    // 4. Bánh xe
    sf::Vector2f wheelSize{10.0f, 4.5f};
    sf::Color wheelColor = sf::Color(20, 20, 20);

    wheel1.setSize(wheelSize); wheel1.setFillColor(wheelColor); wheel1.setOrigin(sf::Vector2f{5.0f, 2.25f});
    wheel2.setSize(wheelSize); wheel2.setFillColor(wheelColor); wheel2.setOrigin(sf::Vector2f{5.0f, 2.25f});
    wheel3.setSize(wheelSize); wheel3.setFillColor(wheelColor); wheel3.setOrigin(sf::Vector2f{5.0f, 2.25f});
    wheel4.setSize(wheelSize); wheel4.setFillColor(wheelColor); wheel4.setOrigin(sf::Vector2f{5.0f, 2.25f});

    // 5. Khởi tạo đèn xi-nhan (màu cam)
    sf::Vector2f signalSize{3.0f, 3.0f};
    sf::Color orangeColor(255, 140, 0);

    leftSignal.setSize(signalSize);
    leftSignal.setFillColor(orangeColor);
    leftSignal.setOrigin(sf::Vector2f{1.5f, 1.5f});

    rightSignal.setSize(signalSize);
    rightSignal.setFillColor(orangeColor);
    rightSignal.setOrigin(sf::Vector2f{1.5f, 1.5f});

    setPosition(startPos);
}

void VehicleRenderer::setPosition(sf::Vector2f pos) {
    float rad = rotationDeg * 3.14159265f / 180.0f;
    float c = std::cos(rad);
    float s = std::sin(rad);

    // Đặt từng bộ phận theo vị trí tương đối so với tâm xe (tương ứng với xe 40x20)
    auto place = [&](sf::RectangleShape& part, float ox, float oy) {
        part.setPosition(sf::Vector2f{pos.x + ox * c - oy * s, pos.y + ox * s + oy * c});
        part.setRotation(sf::degrees(rotationDeg));
    };

        // Bóng đổ lệch cố định về phía dưới-phải màn hình (không xoay theo xe), cùng góc xoay với thân xe
    shadow.setPosition(sf::Vector2f{pos.x + 2.5f, pos.y + 3.5f});
    shadow.setRotation(sf::degrees(rotationDeg));

    place(body,        0.0f,  0.0f);
    place(rearWindow, -9.0f,  0.0f);
    place(roof,       -2.0f,  0.0f);
    place(windshield,  5.5f,  0.0f);
    place(headlight,  18.5f, -6.0f);
    place(headlight2, 18.5f,  6.0f);
    place(taillight1,-18.5f, -6.0f);
    place(taillight2,-18.5f,  6.0f);

    // Vị trí 4 bánh xe co lại cho khớp xe 40x20
    place(wheel1,    11.0f, -11.0f);
    place(wheel2,    11.0f,  11.0f);
    place(wheel3,   -11.0f, -11.0f);
    place(wheel4,   -11.0f,  11.0f);

    // Vị trí 2 xi-nhan đặt ở 2 góc đầu xe phía trước
    place(leftSignal,  18.0f, -8.0f);
    place(rightSignal, 18.0f,  8.0f);
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
        window.draw(shadow);
    window.draw(wheel1); window.draw(wheel2);
    window.draw(wheel3); window.draw(wheel4);
    window.draw(body);
    window.draw(taillight1); window.draw(taillight2);
    window.draw(rearWindow); window.draw(roof); window.draw(windshield);
    window.draw(headlight); window.draw(headlight2);

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