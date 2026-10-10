#pragma once

#include <SFML/Graphics.hpp>

// Trạng thái bật xi-nhan
enum class TurnSignal { None, Left, Right };

class VehicleRenderer {
public:
    sf::RectangleShape body;      // Thân xe
    sf::RectangleShape roof;      // Mui/Kính xe
    sf::RectangleShape wheel1;    // Bánh trước - trên
    sf::RectangleShape wheel2;    // Bánh trước - dưới
    sf::RectangleShape wheel3;    // Bánh sau - trên
    sf::RectangleShape wheel4;    // Bánh sau - dưới
    sf::RectangleShape headlight; // Đèn pha

    // BỔ SUNG ĐÈN XI-NHAN:
    sf::RectangleShape leftSignal;  // Xi-nhan trái
    sf::RectangleShape rightSignal; // Xi-nhan phải
    TurnSignal currentSignal = TurnSignal::None;
    
    // Đồng hồ để nhấp nháy đèn
    sf::Clock blinkClock;
    bool blinkVisible = true;

    VehicleRenderer(sf::Vector2f startPos);
    void setPosition(sf::Vector2f pos);
    void move(sf::Vector2f offset);
    void setTurnSignal(TurnSignal signal); // Bật xi-nhan trái/phải/tắt
    void update();                          // Cập nhật nhấp nháy
    void draw(sf::RenderWindow& window);
    
    sf::Vector2f getPosition() const;
};