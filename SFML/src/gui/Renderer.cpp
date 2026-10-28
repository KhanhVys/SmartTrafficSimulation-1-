#include "Renderer.h"
#include <algorithm>

namespace {
    // Kích thước chuẩn theo CoreConstants.h
    constexpr float WORLD_WIDTH  = 1200.0f;
    constexpr float WORLD_HEIGHT = 800.0f;
}

Renderer::Renderer(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title),
      mapRenderer(static_cast<float>(width), static_cast<float>(height)) {

    window.setFramerateLimit(60);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Khởi tạo vị trí các đèn theo tọa độ hình học chuẩn (vạch dừng Bắc-Nam cách tâm 101, Đông-Tây cách tâm 60)
    // Hướng East (xe chạy sang Tây): Đặt đèn ở phía Đông ngã tư
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx + 60.0f, cy - 101.0f}, false)); // Thẳng
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx + 60.0f, cy - 131.0f}, true));  // Trái

    // Hướng West (xe chạy sang Đông): Đặt đèn ở phía Tây ngã tư
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx - 60.0f, cy + 101.0f}, false)); // Thẳng
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx - 60.0f, cy + 131.0f}, true));  // Trái

    // Hướng North (xe chạy xuống Nam): Đặt đèn ở phía Bắc ngã tư
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx - 101.0f, cy - 60.0f}, false)); // Thẳng
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx - 131.0f, cy - 60.0f}, true));  // Trái

    // Hướng South (xe chạy lên Bắc): Đặt đèn ở phía Nam ngã tư
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx + 101.0f, cy + 60.0f}, false)); // Thẳng
    trafficLights.push_back(TrafficLightRenderer(sf::Vector2f{cx + 131.0f, cy + 60.0f}, true));  // Trái
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
    // Ánh xạ trạng thái đèn từ snapshot thật vào các đối tượng render
    for (size_t i = 0; i < lightSnapshots.size() && i < trafficLights.size(); ++i) {
        const auto& ls = lightSnapshots[i];
        
        // Xử lý đảo hướng đèn Đông-Tây do ánh xạ hướng đi VÀO (Direction) của Core
        // Direction::East nghĩa là xe chạy sang Tây, ta đồng bộ màu đèn tương ứng
        trafficLights[i].setState(ls.color);
    }
}

void Renderer::updateVehicles(const std::vector<VehicleSnapshot>& vehicleSnapshots) {
    std::unordered_map<int, VehicleSnapshot> currentSnapshots;
    for (const auto& vs : vehicleSnapshots) {
        currentSnapshots[vs.id] = vs;
    }

    // Xóa những xe không còn tồn tại trong snapshot mới
    for (auto it = activeVehicles.begin(); it != activeVehicles.end();) {
        if (currentSnapshots.find(it->first) == currentSnapshots.end()) {
            it = activeVehicles.erase(it);
        } else {
            ++it;
        }
    }

    // Cập nhật hoặc thêm mới xe dựa theo snapshot
    for (const auto& vs : vehicleSnapshots) {
        if (activeVehicles.find(vs.id) == activeVehicles.end()) {
            // Xe mới xuất hiện
            activeVehicles[vs.id] = VehicleRenderer(sf::Vector2f{vs.x, vs.y}, vs.direction, vs.movement);
        }

        VehicleRenderer& renderer = activeVehicles[vs.id];
        renderer.setPosition(sf::Vector2f{vs.x, vs.y});
        renderer.setRotation(vs.rotationAngleDegrees);
        renderer.setTurnSignal(vs.signalingLeft ? TurnSignal::Left : TurnSignal::None);
    }
}

void Renderer::render(const SimulationSnapshot& snapshot) {
    // Cập nhật trạng thái từ snapshot thật (GUI không giữ mutex, không ghi dữ liệu mô phỏng)
    updateLights(snapshot.lights);
    updateVehicles(snapshot.vehicles);

    window.clear(sf::Color(35, 110, 35));

    // Vẽ bản đồ ngã tư
    mapRenderer.draw(window);

    // Vẽ đèn giao thông
    for (auto& light : trafficLights) {
        light.draw(window);
    }

    // Vẽ danh sách xe từ snapshot
    for (auto& pair : activeVehicles) {
        pair.second.draw(window);
    }

    window.display();
}