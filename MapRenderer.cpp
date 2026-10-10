#include "MapRenderer.h"

MapRenderer::MapRenderer(float width, float height) {
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // 1. Đường Bắc - Nam (rộng 120px)
    northSouthRoad.setSize(sf::Vector2f{120.0f, height});
    northSouthRoad.setFillColor(sf::Color(50, 50, 50));
    northSouthRoad.setPosition(sf::Vector2f{cx - 60.0f, 0.0f});

    // 2. Đường Đông - Tây (rộng 200px cho 6 làn + dải phân cách)
    eastWestRoad.setSize(sf::Vector2f{width, 200.0f});
    eastWestRoad.setFillColor(sf::Color(50, 50, 50));
    eastWestRoad.setPosition(sf::Vector2f{0.0f, cy - 100.0f});

    // 3. Dải phân cách giữa đường Đông - Tây
    medianStrip.setSize(sf::Vector2f{width, 14.0f});
    medianStrip.setFillColor(sf::Color(100, 100, 100));
    medianStrip.setPosition(sf::Vector2f{0.0f, cy - 7.0f});

    // 4. Vạch kẻ làn đường đứt nét
    std::vector<float> yPositions = {cy - 68.0f, cy - 36.0f, cy + 36.0f, cy + 68.0f};
    for (float x = 0; x < width; x += 35.0f) {
        if (x > cx - 65.0f && x < cx + 65.0f) continue; // Chừa khoảng trắng ở ngã tư
        for (float y : yPositions) {
            sf::RectangleShape line(sf::Vector2f{18.0f, 2.0f});
            line.setFillColor(sf::Color::White);
            line.setPosition(sf::Vector2f{x, y});
            laneLines.push_back(line);
        }
    }

    // Vạch nét đứt đường Bắc - Nam
    for (float y = 0; y < height; y += 35.0f) {
        if (y > cy - 105.0f && y < cy + 105.0f) continue;
        sf::RectangleShape line(sf::Vector2f{2.0f, 18.0f});
        line.setFillColor(sf::Color::Yellow);
        line.setPosition(sf::Vector2f{cx, y});
        laneLines.push_back(line);
    }
}

void MapRenderer::draw(sf::RenderWindow& window) {
    window.draw(northSouthRoad);
    window.draw(eastWestRoad);
    window.draw(medianStrip);
    for (const auto& line : laneLines) {
        window.draw(line);
    }
}