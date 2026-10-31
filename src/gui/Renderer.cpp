#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title),
      mapRenderer(static_cast<float>(width), static_cast<float>(height)) {

    window.setFramerateLimit(60);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Toạ độ pos là góc trên-trái của hộp đèn (24x66 đèn thẳng, 28x28 đèn rẽ).
    // Toàn bộ hộp đèn nằm NGOÀI mép đường (Đ-T: cy±95, B-N: cx±54);
    // đèn rẽ đặt cạnh đèn thẳng theo trục x (cách 30 px) để không che bóng đèn thẳng.
    // Mỗi đèn được gán chặt với (Direction, turnArrow) để nhận đúng trạng thái từ snapshot bất kể thứ tự
    // (số toạ độ có thể chỉnh lại theo mắt sau khi chạy)

       // 1. Hướng East (xe chạy sang Tây): trái của xe là xuống
    trafficLights.push_back({Direction::East, false, TrafficLightRenderer(sf::Vector2f{cx + 60.0f, cy - 165.0f}, false)});
    trafficLights.push_back({Direction::East, true,  TrafficLightRenderer(sf::Vector2f{cx + 90.0f, cy - 165.0f}, true, 270.0f)});

    // 2. Hướng West (xe chạy sang Đông): trái của xe là lên
    trafficLights.push_back({Direction::West, false, TrafficLightRenderer(sf::Vector2f{cx - 84.0f,  cy + 99.0f}, false)});
    trafficLights.push_back({Direction::West, true,  TrafficLightRenderer(sf::Vector2f{cx - 114.0f, cy + 99.0f}, true, 90.0f)});

    // 3. Hướng North (xe chạy xuống Nam): trái của xe là sang phải màn hình
    trafficLights.push_back({Direction::North, false, TrafficLightRenderer(sf::Vector2f{cx - 82.0f,  cy - 167.0f}, false)});
    trafficLights.push_back({Direction::North, true,  TrafficLightRenderer(sf::Vector2f{cx - 112.0f, cy - 167.0f}, true, 180.0f)});

    // 4. Hướng South (xe chạy lên Bắc): trái của xe là sang trái màn hình
    trafficLights.push_back({Direction::South, false, TrafficLightRenderer(sf::Vector2f{cx + 58.0f, cy + 101.0f}, false)});
    trafficLights.push_back({Direction::South, true,  TrafficLightRenderer(sf::Vector2f{cx + 88.0f, cy + 101.0f}, true, 0.0f)});
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
        }
    }
}

void Renderer::updateLights(const std::vector<LightSnapshot>& lightSnapshots) {
    // Khớp đèn dựa trên cặp (direction, turnArrow) từ snapshot thực tế của Core
    for (const auto& ls : lightSnapshots) {
        for (auto& lightMap : trafficLights) {
            if (lightMap.direction == ls.direction && lightMap.turnArrow == ls.turnArrow) {
                lightMap.renderer.setState(ls.color);
                break;
            }
        }
    }
}

void Renderer::updateVehicles(const std::vector<VehicleSnapshot>& vehicleSnapshots) {
    std::unordered_map<int, const VehicleSnapshot*> currentSnapshots;
    for (const auto& vs : vehicleSnapshots) {
        currentSnapshots[vs.vehicleId] = &vs; // Sử dụng đúng trường vehicleId
    }

    // Xóa những xe không còn tồn tại trong snapshot mới
    for (auto it = activeVehicles.begin(); it != activeVehicles.end();) {
        if (currentSnapshots.find(it->first) == currentSnapshots.end()) {
            it = activeVehicles.erase(it);
        } else {
            ++it;
        }
    }

    // Cập nhật hoặc thêm mới xe bằng try_emplace / emplace (vì VehicleRenderer không có constructor mặc định)
    for (const auto& vs : vehicleSnapshots) {
        auto it = activeVehicles.find(vs.vehicleId);
        if (it == activeVehicles.end()) {
            // Thêm mới xe vào pool
            auto res = activeVehicles.try_emplace(
                vs.vehicleId, 
                sf::Vector2f{static_cast<float>(vs.x), static_cast<float>(vs.y)}, 
                vs.direction, 
                vs.movement
            );
            it = res.first;
        }

        // Cập nhật thuộc tính vị trí, góc xoay và xi-nhan
        VehicleRenderer& renderer = it->second;
        renderer.setPosition(sf::Vector2f{static_cast<float>(vs.x), static_cast<float>(vs.y)});
        renderer.setRotation(vs.rotationAngleDegrees);
        renderer.setTurnSignal(vs.signalingLeft ? TurnSignal::Left : TurnSignal::None);
    }
}

void Renderer::render(const SimulationSnapshot& snapshot) {
    updateLights(snapshot.lights);
    updateVehicles(snapshot.vehicles);

    window.clear(sf::Color(35, 110, 35));

    mapRenderer.draw(window);

    // Vẽ đèn giao thông
    for (auto& lightMap : trafficLights) {
        lightMap.renderer.draw(window);
    }

    // Cập nhật hiệu ứng nhấp nháy đèn xi-nhan và vẽ xe
    for (auto& pair : activeVehicles) {
        pair.second.update(); // Gọi update() mỗi frame để xi-nhan nhấp nháy
        pair.second.draw(window);
    }

    window.display();
}