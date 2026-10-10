#include "Renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title),
      mapRenderer(static_cast<float>(width), static_cast<float>(height)) {
    
    window.setFramerateLimit(60);

    float cy = height / 2.0f;

    // XẾP 13 XE NỐI ĐUÔI NHAU CÙNG XUẤT PHÁT TỪ BÊN TRÁI MÀN HÌNH (-150px trở đi)
    // Mỗi xe cách nhau 90px để tạo khoảng cách an toàn
    for (int i = 0; i < 13; ++i) {
        sf::Vector2f startPos{-150.0f - (i * 90.0f), cy + 36.0f}; 
        vehicles.push_back(VehicleRenderer(startPos));
    }

    // Đèn giao thông tại ngã tư
    float cx = width / 2.0f;
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx - 90.0f, cy - 170.0f}));
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx + 70.0f, cy + 105.0f}));
}

bool Renderer::isWindowOpen() const {
    return window.isOpen();
}

void Renderer::handleEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
            // Bấm 'R' để Reset lại toàn bộ xe về vạch xuất phát ban đầu
            else if (keyPressed->code == sf::Keyboard::Key::R) {
                float cy = window.getSize().y / 2.0f;
                for (size_t i = 0; i < vehicles.size(); ++i) {
                    vehicles[i].setPosition(sf::Vector2f{-150.0f - (static_cast<float>(i) * 90.0f), cy + 36.0f});
                }
            }
        }
    }
}

void Renderer::update() {
    float windowWidth = static_cast<float>(window.getSize().x);
    float cy = static_cast<float>(window.getSize().y) / 2.0f;

    // Bật xi-nhan test cho vài xe đầu tiên
    if (!vehicles.empty()) {
        vehicles[0].setTurnSignal(TurnSignal::Left);
        if (vehicles.size() > 1) {
            vehicles[1].setTurnSignal(TurnSignal::Right);
        }
    }

    for (auto& vehicle : vehicles) {
        vehicle.update(); // Cập nhật nhấp nháy xi-nhan
        vehicle.move(sf::Vector2f{2.0f, 0.0f}); // Tốc độ di chuyển

        // NẾU XE CHẠY QUA HẾT MÀP HÌNH BÊN PHẢI -> ĐƯA QUAY TRỞ LẠI BÊN TRÁI ĐỂ CHẠY LẶP LẠI
        if (vehicle.getPosition().x > windowWidth + 100.0f) {
            // Tìm xe có vị trí X nhỏ nhất hiện tại để xếp chiếc này nối đuôi phía sau cùng
            float minX = windowWidth;
            for (const auto& v : vehicles) {
                if (v.getPosition().x < minX) {
                    minX = v.getPosition().x;
                }
            }
            vehicle.setPosition(sf::Vector2f{minX - 90.0f, cy + 36.0f});
        }
    }
}

void Renderer::render() {
    window.clear(sf::Color(35, 110, 35)); // Màu cỏ

    mapRenderer.draw(window);

    for (auto& light : trafficLights) {
        light.draw(window);
    }

    for (auto& vehicle : vehicles) {
        vehicle.draw(window);
    }

    window.display();
}