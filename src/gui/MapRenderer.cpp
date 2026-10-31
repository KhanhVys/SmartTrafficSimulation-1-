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

    // 6a. Vạch dừng đường đôi Đông - Tây
    float stopLenEW = ewHalfRoad - ewMedianHalf; 
    sf::RectangleShape stopEast(sf::Vector2f{4.0f, stopLenEW});   
    stopEast.setFillColor(sf::Color::White);
    stopEast.setPosition(sf::Vector2f{cx - 60.0f, cy + ewMedianHalf});
    stopLines.push_back(stopEast);

    sf::RectangleShape stopWest(sf::Vector2f{4.0f, stopLenEW});   
    stopWest.setFillColor(sf::Color::White);
    stopWest.setPosition(sf::Vector2f{cx + 56.0f, cy - ewHalfRoad});
    stopLines.push_back(stopWest);

    // 6b. Vạch dừng đường Bắc - Nam
    sf::RectangleShape stopSouth(sf::Vector2f{nsHalfRoad, 4.0f});    
    stopSouth.setFillColor(sf::Color::White);
    stopSouth.setPosition(sf::Vector2f{cx - nsHalfRoad, cy - 101.0f});
    stopLines.push_back(stopSouth);

    sf::RectangleShape stopNorth(sf::Vector2f{nsHalfRoad, 4.0f});    
    stopNorth.setFillColor(sf::Color::White);
    stopNorth.setPosition(sf::Vector2f{cx, cy + 97.0f});
    stopLines.push_back(stopNorth);

    // --- Trang trí: Vỉa hè, vạch mép và cây xanh ---
    const float swWidth = 10.0f;
    const sf::Color swColor(150, 150, 150);
    const float nsLeft = cx - nsHalfRoad, nsRight = cx + nsHalfRoad;
    const float ewTop = cy - ewHalfRoad, ewBottom = cy + ewHalfRoad;
    
    auto addRect = [](std::vector<sf::RectangleShape> &v, float x, float y, float w, float h, sf::Color c) {
        sf::RectangleShape r(sf::Vector2f{w, h});
        r.setPosition(sf::Vector2f{x, y});
        r.setFillColor(c);
        v.push_back(r);
    };

    // 1. Vỉa hè (8 đoạn, chừa chỗ ngã tư)
    addRect(sidewalks, 0.0f, ewTop - swWidth, nsLeft, swWidth, swColor);
    addRect(sidewalks, nsRight, ewTop - swWidth, width - nsRight, swWidth, swColor);
    addRect(sidewalks, 0.0f, ewBottom, nsLeft, swWidth, swColor);
    addRect(sidewalks, nsRight, ewBottom, width - nsRight, swWidth, swColor);
    addRect(sidewalks, nsLeft - swWidth, 0.0f, swWidth, ewTop, swColor);
    addRect(sidewalks, nsLeft - swWidth, ewBottom, swWidth, height - ewBottom, swColor);
    addRect(sidewalks, nsRight, 0.0f, swWidth, ewTop, swColor);
    addRect(sidewalks, nsRight, ewBottom, swWidth, height - ewBottom, swColor);

    // 2. Vạch liền trắng ở mép đường
    const sf::Color edgeColor(235, 235, 235);
    addRect(edgeLines, 0.0f, ewTop + 2.0f, nsLeft, 2.0f, edgeColor);
    addRect(edgeLines, nsRight, ewTop + 2.0f, width - nsRight, 2.0f, edgeColor);
    addRect(edgeLines, 0.0f, ewBottom - 4.0f, nsLeft, 2.0f, edgeColor);
    addRect(edgeLines, nsRight, ewBottom - 4.0f, width - nsRight, 2.0f, edgeColor);
    addRect(edgeLines, nsLeft + 2.0f, 0.0f, 2.0f, ewTop, edgeColor);
    addRect(edgeLines, nsLeft + 2.0f, ewBottom, 2.0f, height - ewBottom, edgeColor);
    addRect(edgeLines, nsRight - 4.0f, 0.0f, 2.0f, ewTop, edgeColor);
    addRect(edgeLines, nsRight - 4.0f, ewBottom, 2.0f, height - ewBottom, edgeColor);

    // 3. Cây xanh hai bên đường
    const float dxs[] = {-470.0f, -380.0f, -290.0f, 290.0f, 380.0f, 470.0f};
    const float dys[] = {-190.0f, -280.0f, -360.0f, 190.0f, 280.0f, 360.0f};
    for (float dx : dxs) {
        for (float dy : dys) {
            const int k = ((static_cast<int>(dx + dy) % 3) + 3) % 3;
            const float r = 14.0f + 2.0f * static_cast<float>(k);
            
            sf::CircleShape canopy(r);
            canopy.setOrigin(sf::Vector2f{r, r});
            canopy.setPosition(sf::Vector2f{cx + dx, cy + dy});
            canopy.setFillColor(sf::Color(20, 85, 25));
            trees.push_back(canopy);
            
            sf::CircleShape highlight(r * 0.55f);
            highlight.setOrigin(sf::Vector2f{r * 0.55f, r * 0.55f});
            highlight.setPosition(sf::Vector2f{cx + dx - r * 0.2f, cy + dy - r * 0.2f});
            highlight.setFillColor(sf::Color(40, 130, 45));
            trees.push_back(highlight);
        }
    }
}

void MapRenderer::draw(sf::RenderWindow& window) {
    // Vẽ nền, cây và vỉa hè ở tầng dưới cùng
    for (const auto& t : trees) window.draw(t);
    for (const auto& s : sidewalks) window.draw(s);

    window.draw(northSouthRoad);
    window.draw(eastWestRoad);
    window.draw(medianStrip);

    // Vẽ vạch mép, vạch làn, vạch qua đường và vạch dừng ở tầng trên cùng
    for (const auto& e : edgeLines) window.draw(e);
    for (const auto& line : laneLines) window.draw(line);
    for (const auto& cw : crosswalks)  window.draw(cw);
    for (const auto& sl : stopLines)   window.draw(sl);
}