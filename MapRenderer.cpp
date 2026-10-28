#include "MapRenderer.h"

MapRenderer::MapRenderer(float width, float height) {
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    const float laneWidth  = 48.0f;
    const float medianHalf = 7.0f;
    const float halfRoad   = medianHalf + laneWidth * 3.0f;  // 151px

    // 1. Đường Bắc - Nam (rộng 120px)
    northSouthRoad.setSize(sf::Vector2f{120.0f, height});
    northSouthRoad.setFillColor(sf::Color(50, 50, 50));
    northSouthRoad.setPosition(sf::Vector2f{cx - 60.0f, 0.0f});

    // 2. Đường Đông - Tây (6 làn + dải phân cách)
    eastWestRoad.setSize(sf::Vector2f{width, halfRoad * 2.0f});
    eastWestRoad.setFillColor(sf::Color(50, 50, 50));
    eastWestRoad.setPosition(sf::Vector2f{0.0f, cy - halfRoad});

    // 3. Dải phân cách
    medianStrip.setSize(sf::Vector2f{width, medianHalf * 2.0f});
    medianStrip.setFillColor(sf::Color(100, 100, 100));
    medianStrip.setPosition(sf::Vector2f{0.0f, cy - medianHalf});

    // 4. Vạch kẻ làn đứt nét đường Đông - Tây
    float l1 = medianHalf + laneWidth;         // 55
    float l2 = medianHalf + laneWidth * 2.0f;  // 103
    std::vector<float> yPositions = {cy - l2, cy - l1, cy + l1, cy + l2};
    for (float x = 0; x < width; x += 70.0f) {
        if (x > cx - 115.0f && x < cx + 90.0f) continue; // Chừa ngã tư + vạch qua đường + vạch dừng
        for (float y : yPositions) {
            sf::RectangleShape line(sf::Vector2f{24.0f, 2.0f});
            line.setFillColor(sf::Color::White);
            line.setPosition(sf::Vector2f{x, y - 1.0f});
            laneLines.push_back(line);
        }
    }

    // Vạch nét đứt đường Bắc - Nam
    for (float y = 0; y < height; y += 70.0f) {
        if (y > cy - (halfRoad + 50.0f) && y < cy + (halfRoad + 30.0f)) continue;
        sf::RectangleShape line(sf::Vector2f{2.0f, 24.0f});
        line.setFillColor(sf::Color::Yellow);
        line.setPosition(sf::Vector2f{cx - 1.0f, y});
        laneLines.push_back(line);
    }

    sf::Color cwColor(220, 220, 220, 200);

    // 5a. Vạch qua đường cắt ngang đường Bắc - Nam (phía trên và dưới ngã tư)
    for (float x = cx - 55.0f; x < cx + 55.0f; x += 14.0f) {
        sf::RectangleShape cwTop(sf::Vector2f{8.0f, 15.0f});
        cwTop.setFillColor(cwColor);
        cwTop.setPosition(sf::Vector2f{x, cy - halfRoad - 16.0f});
        crosswalks.push_back(cwTop);

        sf::RectangleShape cwBottom(sf::Vector2f{8.0f, 15.0f});
        cwBottom.setFillColor(cwColor);
        cwBottom.setPosition(sf::Vector2f{x, cy + halfRoad + 1.0f});
        crosswalks.push_back(cwBottom);
    }

    // 5b. Vạch qua đường cắt ngang đường đôi Đông - Tây (bên trái và phải ngã tư)
    for (float y = cy - halfRoad + 4.0f; y < cy + halfRoad - 8.0f; y += 14.0f) {
        sf::RectangleShape cwLeft(sf::Vector2f{15.0f, 8.0f});
        cwLeft.setFillColor(cwColor);
        cwLeft.setPosition(sf::Vector2f{cx - 76.0f, y});
        crosswalks.push_back(cwLeft);

        sf::RectangleShape cwRight(sf::Vector2f{15.0f, 8.0f});
        cwRight.setFillColor(cwColor);
        cwRight.setPosition(sf::Vector2f{cx + 61.0f, y});
        crosswalks.push_back(cwRight);
    }

    // 6a. Vạch dừng đường đôi Đông - Tây (lùi ra trước vạch qua đường)
    float stopLen = halfRoad - medianHalf; // 144
    sf::RectangleShape stopEast(sf::Vector2f{4.0f, stopLen});   // xe đi hướng Đông (nửa dưới)
    stopEast.setFillColor(sf::Color::White);
    stopEast.setPosition(sf::Vector2f{cx - 84.0f, cy + medianHalf});
    stopLines.push_back(stopEast);

    sf::RectangleShape stopWest(sf::Vector2f{4.0f, stopLen});   // xe đi hướng Tây (nửa trên)
    stopWest.setFillColor(sf::Color::White);
    stopWest.setPosition(sf::Vector2f{cx + 80.0f, cy - halfRoad});
    stopLines.push_back(stopWest);

    // 6b. Vạch dừng đường Bắc - Nam
    sf::RectangleShape stopSouth(sf::Vector2f{56.0f, 4.0f});    // xe đi xuống (nửa trái)
    stopSouth.setFillColor(sf::Color::White);
    stopSouth.setPosition(sf::Vector2f{cx - 58.0f, cy - halfRoad - 24.0f});
    stopLines.push_back(stopSouth);

    sf::RectangleShape stopNorth(sf::Vector2f{56.0f, 4.0f});    // xe đi lên (nửa phải)
    stopNorth.setFillColor(sf::Color::White);
    stopNorth.setPosition(sf::Vector2f{cx + 2.0f, cy + halfRoad + 20.0f});
    stopLines.push_back(stopNorth);
}

void MapRenderer::draw(sf::RenderWindow& window) {
    window.draw(northSouthRoad);
    window.draw(eastWestRoad);
    window.draw(medianStrip);

    for (const auto& line : laneLines) window.draw(line);
    for (const auto& cw : crosswalks)  window.draw(cw);
    for (const auto& sl : stopLines)   window.draw(sl);
}
