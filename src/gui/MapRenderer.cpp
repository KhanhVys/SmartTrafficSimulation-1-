#include "MapRenderer.h"

MapRenderer::MapRenderer(float width, float height) {
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // --- Thông số hình học chuẩn theo CoreConstants.h ---
    const float ewLaneWidth  = 30.0f;   // Đường Đông-Tây: làn 30 px
    const float ewMedianHalf = 5.0f;    // Dải giữa nửa rộng 5 px (tổng 10 px)
    const float ewHalfRoad   = ewMedianHalf + ewLaneWidth * 3.0f;  // 5 + 90 = 95px mỗi nửa

    const float nsLaneWidth  = 26.0f;   // Đường Bắc-Nam: mỗi hàng 26 px
    const float nsHalfRoad   = nsLaneWidth * 2.0f + 2.0f;          // Tổng rộng 108px (mỗi bên 54px, nửa khoảng giữa 2px)

    // 1. Đường Bắc - Nam (rộng 108px)
    northSouthRoad.setSize(sf::Vector2f{nsHalfRoad * 2.0f, height});
    northSouthRoad.setFillColor(sf::Color(50, 50, 50));
    northSouthRoad.setPosition(sf::Vector2f{cx - nsHalfRoad, 0.0f});

    // 2. Đường Đông - Tây (3 làn mỗi chiều + dải phân cách)
    eastWestRoad.setSize(sf::Vector2f{width, ewHalfRoad * 2.0f});
    eastWestRoad.setFillColor(sf::Color(50, 50, 50));
    eastWestRoad.setPosition(sf::Vector2f{0.0f, cy - ewHalfRoad});

    // 3. Dải phân cách giữa đường Đông - Tây
    medianStrip.setSize(sf::Vector2f{width, ewMedianHalf * 2.0f});
    medianStrip.setFillColor(sf::Color(100, 100, 100));
    medianStrip.setPosition(sf::Vector2f{0.0f, cy - ewMedianHalf});

    // 4. Vạch kẻ làn đứt nét đường Đông - Tây
    float l1 = ewMedianHalf + ewLaneWidth;         // 35
    float l2 = ewMedianHalf + ewLaneWidth * 2.0f;  // 65
    std::vector<float> yPositions = {cy - l2, cy - l1, cy + l1, cy + l2};
    for (float x = 0; x < width; x += 70.0f) {
        if (x > cx - nsHalfRoad - 15.0f && x < cx + nsHalfRoad + 15.0f) continue; 
        for (float y : yPositions) {
            sf::RectangleShape line(sf::Vector2f{24.0f, 2.0f});
            line.setFillColor(sf::Color::White);
            line.setPosition(sf::Vector2f{x, y - 1.0f});
            laneLines.push_back(line);
        }
    }

    // Vạch nét đứt phân chia chiều đường Bắc - Nam
    for (float y = 0; y < height; y += 70.0f) {
        if (y > cy - (ewHalfRoad + 50.0f) && y < cy + (ewHalfRoad + 30.0f)) continue;
        sf::RectangleShape line(sf::Vector2f{2.0f, 24.0f});
        line.setFillColor(sf::Color::Yellow);
        line.setPosition(sf::Vector2f{cx - 1.0f, y});
        laneLines.push_back(line);
    }

    sf::Color cwColor(220, 220, 220, 200);

    // 5a. Vạch qua đường cắt ngang đường Bắc - Nam (phía trên và dưới ngã tư)
    // Vạch dừng B-N nằm ở cy-101 và cy+101 (mép ngoài); dải vạch qua đường dời ra ngoài thêm 8 px
    // -> cwTop chiếm cy-124..cy-109, cwBottom chiếm cy+109..cy+124
    for (float x = cx - nsHalfRoad; x < cx + nsHalfRoad; x += 14.0f) {
        sf::RectangleShape cwTop(sf::Vector2f{8.0f, 15.0f});
        cwTop.setFillColor(cwColor);
        cwTop.setPosition(sf::Vector2f{x, cy - 94.0f});
        crosswalks.push_back(cwTop);

        sf::RectangleShape cwBottom(sf::Vector2f{8.0f, 15.0f});
        cwBottom.setFillColor(cwColor);
        cwBottom.setPosition(sf::Vector2f{x, cy + 79.0f});
        crosswalks.push_back(cwBottom);
    }

    // 5b. Vạch qua đường cắt ngang đường đôi Đông - Tây (bên trái và phải ngã tư)
    for (float y = cy - 77.0f; y < cy + 70.0f; y += 14.0f) {
        sf::RectangleShape cwLeft(sf::Vector2f{15.0f, 8.0f});
        cwLeft.setFillColor(cwColor);
        cwLeft.setPosition(sf::Vector2f{cx - nsHalfRoad + 1.0f, y});
        crosswalks.push_back(cwLeft);

        sf::RectangleShape cwRight(sf::Vector2f{15.0f, 8.0f});
        cwRight.setFillColor(cwColor);
        cwRight.setPosition(sf::Vector2f{cx + nsHalfRoad - 16.0f, y});
        crosswalks.push_back(cwRight);
    }

    // 6a. Vạch dừng đường đôi Đông - Tây (Cách tâm đúng 60px theo yêu cầu)
    float stopLenEW = ewHalfRoad - ewMedianHalf; 
    sf::RectangleShape stopEast(sf::Vector2f{4.0f, stopLenEW});   // xe đi hướng Đông (nửa dưới)
    stopEast.setFillColor(sf::Color::White);
    stopEast.setPosition(sf::Vector2f{cx - 60.0f, cy + ewMedianHalf});
    stopLines.push_back(stopEast);

    sf::RectangleShape stopWest(sf::Vector2f{4.0f, stopLenEW});   // xe đi hướng Tây (nửa trên)
    stopWest.setFillColor(sf::Color::White);
    stopWest.setPosition(sf::Vector2f{cx + 56.0f, cy - ewHalfRoad});
    stopLines.push_back(stopWest);

    // 6b. Vạch dừng đường Bắc - Nam (Cách tâm đúng 101px theo yêu cầu)
    sf::RectangleShape stopSouth(sf::Vector2f{nsHalfRoad, 4.0f});    // xe đi xuống (nửa trái)
    stopSouth.setFillColor(sf::Color::White);
    stopSouth.setPosition(sf::Vector2f{cx - nsHalfRoad, cy - 101.0f});
    stopLines.push_back(stopSouth);

    sf::RectangleShape stopNorth(sf::Vector2f{nsHalfRoad, 4.0f});    // xe đi lên (nửa phải)
    stopNorth.setFillColor(sf::Color::White);
    stopNorth.setPosition(sf::Vector2f{cx, cy + 97.0f});
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