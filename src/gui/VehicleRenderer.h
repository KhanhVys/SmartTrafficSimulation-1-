#ifndef VEHICLE_RENDERER_H
#define VEHICLE_RENDERER_H

#include <SFML/Graphics.hpp>
#include "common/Types.h"   // Direction, Movement dùng chung toàn dự án

enum class TurnSignal {
    None,
    Left,
    Right
};

class VehicleRenderer {
private:
    sf::RectangleShape body;
    sf::RectangleShape roof;
    sf::RectangleShape headlight;
    sf::RectangleShape wheel1, wheel2, wheel3, wheel4;
    sf::RectangleShape leftSignal, rightSignal;

    TurnSignal currentSignal = TurnSignal::None;
    sf::Clock blinkClock;
    bool blinkVisible = true;

    // Bổ sung thuộc tính hướng và chuyển động theo quy chuẩn mô phỏng
    Direction direction;
    Movement movement;

    // Góc quay của xe (độ, 0 = đầu xe hướng sang phải/Đông, quay theo chiều kim đồng hồ)
    float rotationDeg = 0.0f;

public:
    VehicleRenderer(sf::Vector2f startPos, Direction dir = Direction::East, Movement mov = Movement::Straight);

    void setPosition(sf::Vector2f pos);
    void move(sf::Vector2f offset);
    void setRotation(float degrees);
    void setTurnSignal(TurnSignal signal);
    void setColorById(int vehicleId); // chọn màu thân xe theo vehicleId % số màu
    void update();
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    float getRotation() const;

    // Các hàm getter/setter hỗ trợ đồng bộ với luồng mô phỏng
    Direction getDirection() const;
    Movement getMovement() const;
};

#endif // VEHICLE_RENDERER_H