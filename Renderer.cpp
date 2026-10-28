#include "Renderer.h"

#include <algorithm>
#include <cmath>

namespace {
    constexpr LightColor R = LightColor::Red;
    constexpr LightColor Y = LightColor::Yellow;
    constexpr LightColor G = LightColor::Green;

    // Các pha theo bảng chu kỳ: {giây, BN-Thẳng, BN-Trái/Quay, ĐT-Thẳng, ĐT-Trái/Quay}
    struct LightPhase {
        float duration;
        LightColor nsStraight, nsLeft, ewStraight, ewLeft;
    };

    const std::vector<LightPhase> PHASES = {
        {6.0f, G, R, R, R},  // 0–5   BN Thẳng xanh
        {2.0f, Y, R, R, R},  // 6–7   BN Thẳng vàng
        {2.0f, R, R, R, R},  // 8–9   Đỏ toàn bộ
        {5.0f, R, G, R, R},  // 10–14 BN Trái+Quay xanh
        {2.0f, R, Y, R, R},  // 15–16 BN Trái+Quay vàng
        {2.0f, R, R, R, R},  // 17–18 Đỏ toàn bộ
        {6.0f, R, R, G, R},  // 19–24 ĐT Thẳng xanh
        {2.0f, R, R, Y, R},  // 25–26 ĐT Thẳng vàng
        {2.0f, R, R, R, R},  // 27–28 Đỏ toàn bộ
        {5.0f, R, R, R, G},  // 29–33 ĐT Trái+Quay xanh
        {2.0f, R, R, R, Y},  // ĐT Trái+Quay vàng
        {2.0f, R, R, R, R},  // Đỏ toàn bộ
    };

    void setAll(std::vector<TrafficLightRenderer>& group, LightColor c) {
        for (auto& l : group) l.setState(c);
    }

    // ---- Hình học đường (phải khớp với MapRenderer) ----
    constexpr float PI           = 3.14159265f;
    constexpr float LANE_WIDTH   = 48.0f;
    constexpr float MEDIAN_HALF  = 7.0f;
    constexpr float STOP_OFFSET  = 84.0f;   // vạch dừng Đông-Tây nằm ở cx - 84
    constexpr float NORTH_LANE_X = 30.0f;   // tâm làn đi lên của đường Bắc-Nam: cx + 30

    // ---- Chạy xe ----
    constexpr float SPEED    = 2.0f;
    constexpr float CAR_GAP  = 130.0f;      // khoảng cách khi sinh xe
    constexpr float MIN_GAP  = 105.0f;      // khoảng cách an toàn với xe phía trước
    constexpr float CAR_HALF = 25.0f;       // nửa chiều dài xe

    // ---- Đường rẽ ----
    constexpr float LEFT_RADIUS   = 60.0f;                            // bán kính rẽ trái
    constexpr float UTURN_RADIUS  = MEDIAN_HALF + LANE_WIDTH / 2.0f;  // 31: từ làn trong xuống làn trong ngược chiều
    constexpr float UTURN_START_X = -20.0f;                           // điểm bắt đầu quay đầu: cx - 20

    // Tâm làn thứ `lane` của nửa đường phía dưới (xe đi hướng Đông)
    float laneCenterY(float cy, int lane) {
        return cy + MEDIAN_HALF + LANE_WIDTH * (static_cast<float>(lane) + 0.5f);
    }

    // 13 xe thử nghiệm: làn trong thử rẽ trái/quay đầu, làn giữa và ngoài đi thẳng
    struct CarSpec { int lane; Movement movement; };
    const std::vector<CarSpec> TEST_CARS = {
        {0, Movement::Left},  {1, Movement::Straight}, {2, Movement::Straight},
        {0, Movement::UTurn}, {1, Movement::Straight}, {2, Movement::Straight},
        {0, Movement::Left},  {1, Movement::Straight}, {2, Movement::Straight},
        {0, Movement::UTurn}, {1, Movement::Straight}, {2, Movement::Straight},
        {1, Movement::Straight},
    };

    // Xe rẽ trái/quay đầu theo đèn trái, còn lại theo đèn thẳng
    LightColor lightFor(Movement m, const LightPhase& p) {
        return (m == Movement::Left || m == Movement::UTurn) ? p.ewLeft : p.ewStraight;
    }
}

Renderer::Renderer(unsigned int width, unsigned int height, const std::string& title)
    : window(sf::VideoMode({width, height}), title),
      mapRenderer(static_cast<float>(width), static_cast<float>(height)) {

    window.setFramerateLimit(60);

    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Tạo 13 xe theo danh sách thử nghiệm (vị trí được đặt trong resetVehicles)
    for (const auto& spec : TEST_CARS) {
        vehicles.push_back(VehicleRenderer(sf::Vector2f{0.0f, 0.0f}, Direction::East, spec.movement));
        motions.push_back(CarMotion{});
    }
    resetVehicles();

    // BN-Thẳng: xe đi xuống (phía trên, bên trái) và xe đi lên (phía dưới, bên phải)
    lightsNSStraight.push_back(TrafficLightRenderer(sf::Vector2f{cx - 92.0f,  cy - 222.0f}));
    lightsNSStraight.push_back(TrafficLightRenderer(sf::Vector2f{cx + 68.0f,  cy + 157.0f}));
    // BN-Trái/Quay
    lightsNSLeft.push_back(TrafficLightRenderer(sf::Vector2f{cx - 132.0f, cy - 222.0f}, true));
    lightsNSLeft.push_back(TrafficLightRenderer(sf::Vector2f{cx + 108.0f, cy + 157.0f}, true));

    // ĐT-Thẳng: xe đi sang Đông (bên trái ngã tư) và xe đi sang Tây (bên phải ngã tư)
    lightsEWStraight.push_back(TrafficLightRenderer(sf::Vector2f{cx - 150.0f, cy + 157.0f}));
    lightsEWStraight.push_back(TrafficLightRenderer(sf::Vector2f{cx + 100.0f, cy - 222.0f}));
    // ĐT-Trái/Quay
    lightsEWLeft.push_back(TrafficLightRenderer(sf::Vector2f{cx - 190.0f, cy + 157.0f}, true));
    lightsEWLeft.push_back(TrafficLightRenderer(sf::Vector2f{cx + 140.0f, cy - 222.0f}, true));
}

void Renderer::resetVehicles() {
    float cy = window.getSize().y / 2.0f;
    int perLane[3] = {0, 0, 0};

    for (size_t i = 0; i < vehicles.size(); ++i) {
        int lane = TEST_CARS[i].lane;
        float x = -180.0f - static_cast<float>(perLane[lane]++) * CAR_GAP;

        vehicles[i].setRotation(0.0f);
        vehicles[i].setPosition(sf::Vector2f{x, laneCenterY(cy, lane)});
        vehicles[i].setTurnSignal(TurnSignal::None);
        motions[i] = CarMotion{lane, 0, 0.0f};
    }

    phaseIndex = 0;
    cycleClock.restart();
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
            else if (keyPressed->code == sf::Keyboard::Key::R) {
                resetVehicles();
            }
            else if (keyPressed->code == sf::Keyboard::Key::N) {
                // Bỏ qua sang pha đèn kế tiếp (tiện để thử rẽ trái/quay đầu)
                phaseIndex = (phaseIndex + 1) % PHASES.size();
                cycleClock.restart();
            }
        }
    }
}

void Renderer::update() {
    float windowWidth  = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float cx = windowWidth / 2.0f;
    float cy = windowHeight / 2.0f;

    float stopLineX = cx - STOP_OFFSET;

    // Chuyển pha khi hết thời gian
    if (cycleClock.getElapsedTime().asSeconds() > PHASES[phaseIndex].duration) {
        phaseIndex = (phaseIndex + 1) % PHASES.size();
        cycleClock.restart();
    }

    const LightPhase& phase = PHASES[phaseIndex];
    setAll(lightsNSStraight, phase.nsStraight);
    setAll(lightsNSLeft,     phase.nsLeft);
    setAll(lightsEWStraight, phase.ewStraight);
    setAll(lightsEWLeft,     phase.ewLeft);

    // Đưa xe về cuối hàng của làn mình sau khi ra khỏi màn hình
    auto respawn = [&](size_t i) {
        int lane = motions[i].lane;
        float lastX = 0.0f;  // không bao giờ sinh xe trong màn hình
        for (size_t j = 0; j < vehicles.size(); ++j) {
            if (j != i && motions[j].lane == lane && motions[j].phase == 0) {
                lastX = std::min(lastX, vehicles[j].getPosition().x);
            }
        }
        vehicles[i].setRotation(0.0f);
        vehicles[i].setPosition(sf::Vector2f{lastX - CAR_GAP, laneCenterY(cy, lane)});
        vehicles[i].setTurnSignal(TurnSignal::None);
        motions[i].phase = 0;
        motions[i].arc = 0.0f;
    };

    for (size_t i = 0; i < vehicles.size(); ++i) {
        VehicleRenderer& car = vehicles[i];
        CarMotion& m = motions[i];
        Movement mov = car.getMovement();
        bool turning = (mov == Movement::Left || mov == Movement::UTurn);

        car.update();
        sf::Vector2f pos = car.getPosition();

        if (m.phase == 0) {
            // ---- Đang tiến tới ngã tư theo làn ----
            if (turning && pos.x > stopLineX - 260.0f) {
                car.setTurnSignal(TurnSignal::Left);   // bật xi-nhan trước khi rẽ
            }

            bool shouldStop = false;

            // Đèn không xanh và đầu xe chưa qua vạch dừng => dừng; đã qua vạch thì đi tiếp
            float front = pos.x + CAR_HALF;
            if (lightFor(mov, phase) != LightColor::Green && front < stopLineX && front + 12.0f >= stopLineX) {
                shouldStop = true;
            }

            // Giữ khoảng cách với xe phía trước cùng làn (chỉ xét xe chưa bắt đầu rẽ)
            for (size_t j = 0; j < vehicles.size(); ++j) {
                if (j == i || motions[j].lane != m.lane || motions[j].phase != 0) continue;
                float dx = vehicles[j].getPosition().x - pos.x;
                if (dx > 0.0f && dx < MIN_GAP) {
                    shouldStop = true;
                    break;
                }
            }

            if (!shouldStop) {
                car.move(sf::Vector2f{SPEED, 0.0f});
            }

            // Bắt đầu rẽ khi tới điểm đầu của cung tròn
            pos = car.getPosition();
            float turnStartX = (mov == Movement::Left) ? (cx + NORTH_LANE_X - LEFT_RADIUS)
                                                       : (cx + UTURN_START_X);
            if (turning && pos.x >= turnStartX) {
                m.phase = 1;
                m.arc = 0.0f;
            }

            // Xe đi thẳng ra khỏi màn hình bên phải
            if (!turning && car.getPosition().x > windowWidth + 100.0f) {
                respawn(i);
            }
        }
        else if (m.phase == 1) {
            // ---- Đang rẽ theo cung tròn ----
            bool isLeft = (mov == Movement::Left);
            float radius = isLeft ? LEFT_RADIUS : UTURN_RADIUS;
            float totalAngle = isLeft ? (PI / 2.0f) : PI;
            sf::Vector2f center = isLeft
                ? sf::Vector2f{cx + NORTH_LANE_X - LEFT_RADIUS, laneCenterY(cy, 0) - LEFT_RADIUS}
                : sf::Vector2f{cx + UTURN_START_X, cy};

            m.arc += SPEED;
            float done = std::min(m.arc / radius, totalAngle);
            float theta = PI / 2.0f - done;   // 90° = ngay dưới tâm (đầu cung, xe hướng Đông)

            car.setRotation(theta * 180.0f / PI - 90.0f);
            car.setPosition(sf::Vector2f{center.x + radius * std::cos(theta),
                                         center.y + radius * std::sin(theta)});

            if (done >= totalAngle) {
                m.phase = 2;
                car.setTurnSignal(TurnSignal::None);
            }
        }
        else {
            // ---- Đã rẽ xong, chạy thẳng ra khỏi màn hình ----
            if (mov == Movement::Left) {
                car.move(sf::Vector2f{0.0f, -SPEED});   // đi lên (hướng Bắc)
            } else {
                car.move(sf::Vector2f{-SPEED, 0.0f});   // quay đầu: đi sang Tây
            }

            pos = car.getPosition();
            if (pos.y < -100.0f || pos.x < -100.0f) {
                respawn(i);
            }
        }
    }
}

void Renderer::render(const SimulationSnapshot&)  {
    window.clear(sf::Color(35, 110, 35));

    mapRenderer.draw(window);

    for (auto& l : lightsNSStraight) l.draw(window);
    for (auto& l : lightsNSLeft)     l.draw(window);
    for (auto& l : lightsEWStraight) l.draw(window);
    for (auto& l : lightsEWLeft)     l.draw(window);

    for (auto& vehicle : vehicles) {
        vehicle.draw(window);
    }

    window.display();
}
