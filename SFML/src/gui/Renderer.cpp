#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title),
      mapRenderer(static_cast<float>(width), static_cast<float>(height)) {

    window.setFramerateLimit(60);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Khởi tạo vị trí các đèn giao thông theo đúng tọa độ chuẩn (vạch dừng Bắc-Nam cách tâm 101, Đông-Tây cách tâm 60)
    // Mỗi đèn được gán chặt với (Direction, turnArrow) để nhận đúng trạng thái từ snapshot bất kể thứ tự
    
    // 1. Hướng East (xe chạy sang Tây)
    trafficLights.push_back({Direction::East, false, TrafficLightRenderer(sf::Vector2f{cx + 60.0f, cy - 101.0f}, false)});
    trafficLights.push_back({Direction::East, true,  TrafficLightRenderer(sf::Vector2f{cx + 60.0f, cy - 131.0f}, true)});

    // 2. Hướng West (xe chạy sang Đông)
    trafficLights.push_back({Direction::West, false, TrafficLightRenderer(sf::Vector2f{cx - 60.0f, cy + 101.0f}, false)});
    trafficLights.push_back({Direction::West, true,  TrafficLightRenderer(sf::Vector2f{cx - 60.0f, cy + 131.0f}, true)});

    // 3. Hướng North (xe chạy xuống Nam)
    trafficLights.push_back({Direction::North, false, TrafficLightRenderer(sf::Vector2f{cx - 101.0f, cy - 60.0f}, false)});
    trafficLights.push_back({Direction::North, true,  TrafficLightRenderer(sf::Vector2f{cx - 131.0f, cy - 60.0f}, true)});

    // 4. Hướng South (xe chạy lên Bắc)
    trafficLights.push_back({Direction::South, false, TrafficLightRenderer(sf::Vector2f{cx + 101.0f, cy + 60.0f}, false)});
    trafficLights.push_back({Direction::South, true,  TrafficLightRenderer(sf::Vector2f{cx + 131.0f, cy + 60.0f}, true)});
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
                sf::Vector2f{vs.x, vs.y}, 
                vs.direction, 
                vs.movement
            );
            it = res.first;
        }

        // Cập nhật thuộc tính vị trí, góc xoay và xi-nhan
        VehicleRenderer& renderer = it->second;
        renderer.setPosition(sf::Vector2f{vs.x, vs.y});
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