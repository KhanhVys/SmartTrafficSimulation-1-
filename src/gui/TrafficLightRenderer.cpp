#include "TrafficLightRenderer.h"

<<<<<<< HEAD
// Góc xoay mũi tên rẽ trái (độ, chiều kim đồng hồ; 0 độ = mũi tên chỉ sang trái màn hình).
// Mũi tên chỉ về bên trái của tài xế khi xe đi vào ngã tư từ nhánh `approach`:
//   South (xe chạy lên)       -> sang trái (Tây)
//   West  (xe chạy sang Đông) -> lên       (Bắc)
//   North (xe chạy xuống)     -> sang phải (Đông)
//   East  (xe chạy sang Tây)  -> xuống     (Nam)
static float leftArrowAngle(Direction approach) {
    switch (approach) {
        case Direction::South: return 0.0f;
        case Direction::West:  return 90.0f;
        case Direction::North: return 180.0f;
        case Direction::East:  return 270.0f;
    }
    return 0.0f;
}

TrafficLightRenderer::TrafficLightRenderer(sf::Vector2f pos, bool leftTurn, Direction approach)
=======
TrafficLightRenderer::TrafficLightRenderer(sf::Vector2f pos, bool leftTurn, float arrowAngleDeg)
>>>>>>> 7a72981f201c9a97a5e645f5208b00e8f59042fc
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

<<<<<<< HEAD
        // Mũi tên vẽ sẵn theo hướng "sang trái", rồi xoay quanh tâm đèn theo nhánh đi vào.
        const sf::Vector2f center{ox + 14.5f, oy + 14.0f};
        const sf::Angle angle = sf::degrees(leftArrowAngle(approach));

        arrowShaft.setSize(sf::Vector2f{9.0f, 4.0f});
        arrowShaft.setOrigin(sf::Vector2f{0.5f, 2.0f});
        arrowShaft.setPosition(center);
        arrowShaft.setRotation(angle);
=======
        // Tâm đèn: mũi tên xoay quanh điểm này
        sf::Vector2f center{ox + 14.0f, oy + 14.0f};
>>>>>>> 7a72981f201c9a97a5e645f5208b00e8f59042fc

        // Thân mũi tên (gốc xoay ở đầu thân, đặt đúng tâm đèn)
        arrowShaft.setSize(sf::Vector2f{9.0f, 4.0f});
        arrowShaft.setOrigin(sf::Vector2f{0.0f, 2.0f});
        arrowShaft.setPosition(center);
        arrowShaft.setRotation(sf::degrees(arrowAngleDeg));

        // Đầu mũi tên (gốc xoay đặt đúng tâm đèn)
        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f{0.0f, 7.0f});        // đầu mũi tên
        arrow.setPoint(1, sf::Vector2f{8.0f, 0.0f});
        arrow.setPoint(2, sf::Vector2f{8.0f, 14.0f});
<<<<<<< HEAD
        arrow.setOrigin(sf::Vector2f{8.5f, 7.0f});
        arrow.setPosition(center);
        arrow.setRotation(angle);
=======
        arrow.setOrigin(sf::Vector2f{8.0f, 7.0f});
        arrow.setPosition(center);
        arrow.setRotation(sf::degrees(arrowAngleDeg));
>>>>>>> 7a72981f201c9a97a5e645f5208b00e8f59042fc
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
        
        glowCenter = arrowLamp.getPosition() + sf::Vector2f{11.0f, 11.0f}; // tâm bóng (bán kính 11)
        glowColor = c;
        glowOn = true;
        return;
    }

    sf::Color offColor(60, 60, 60);

    redLight.setFillColor(state == LightColor::Red ? sf::Color::Red : offColor);
    yellowLight.setFillColor(state == LightColor::Yellow ? sf::Color::Yellow : offColor);
    greenLight.setFillColor(state == LightColor::Green ? sf::Color::Green : offColor);
    const sf::CircleShape& lit = state == LightColor::Red ? redLight : state == LightColor::Yellow ? yellowLight : greenLight;
    glowCenter = lit.getPosition() + sf::Vector2f{7.0f, 7.0f}; // tâm bóng (bán kính 7)
    glowColor = state == LightColor::Red ? sf::Color::Red : state == LightColor::Yellow ? sf::Color::Yellow : sf::Color::Green;
    glowOn = true;
}

void TrafficLightRenderer::draw(sf::RenderWindow& window) {
    window.draw(box);
auto drawGlow = [&] {
    if (!glowOn) return;
    // 3 vòng tròn đồng tâm, càng ra ngoài càng mờ, cộng màu cho cảm giác phát sáng
    const float radii[] = {13.0f, 19.0f, 26.0f};
    const std::uint8_t alphas[] = {70, 38, 16};
    for (int i = 0; i < 3; ++i) {
        sf::CircleShape g(radii[i]);
        g.setOrigin({radii[i], radii[i]});
        g.setPosition(glowCenter);
        g.setFillColor(sf::Color(glowColor.r, glowColor.g, glowColor.b, alphas[i]));
        window.draw(g, sf::BlendAdd);
    }
};
    if (hasArrow) {
        window.draw(arrowLamp);
        window.draw(arrowShaft);
        window.draw(arrow);
        drawGlow();
        return;
    }

    window.draw(redLight);
    window.draw(yellowLight);
    window.draw(greenLight);
    drawGlow();
}