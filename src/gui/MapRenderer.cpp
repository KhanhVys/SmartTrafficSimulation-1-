#include "MapRenderer.h"
#include <algorithm>
#include <cmath>

namespace {
    // Tạo 1 "khối" nằm ở một góc ngã tư, cạnh trong bo tròn.
    // (ix, iy): góc trong của khối (sát ngã tư). (sx, sy): hướng đi ra xa ngã tư, mỗi giá trị là +1 hoặc -1.
    // inset: thụt vào bao nhiêu px so với mép đường. radius: bán kính bo góc ở mép đường (inset = 0).
        sf::ConvexShape makeBlock(float ix, float iy, float sx, float sy,
                              float inset, float radius, sf::Color fill) {
        const float BIG = 3000.0f;                        // đủ lớn để tràn ra ngoài cửa sổ
        const float r = std::max(radius - inset, 0.0f);   // bán kính cung sau khi thụt vào (về 0 thì góc vuông)
        const float off = inset + r;                      // khoảng cách từ mép đường tới tâm cung
        const float ax = ix + sx * off;                   // tâm cung tròn
        const float ay = iy + sy * off;
        const int steps = (r > 0.01f) ? 8 : 0;

        sf::ConvexShape shape;
        shape.setPointCount(static_cast<std::size_t>(steps + 1 + 3));
        std::size_t k = 0;
        shape.setPoint(k++, sf::Vector2f{ix + sx * BIG, iy + sy * inset});
        for (int i = 0; i <= steps; ++i) {
            float t = (steps > 0) ? 1.5707963f * static_cast<float>(i) / static_cast<float>(steps) : 0.0f; // 0 -> 90 độ
            shape.setPoint(k++, sf::Vector2f{ax - sx * r * std::sin(t), ay - sy * r * std::cos(t)});
        }
        shape.setPoint(k++, sf::Vector2f{ix + sx * inset, iy + sy * BIG});
        shape.setPoint(k++, sf::Vector2f{ix + sx * BIG,   iy + sy * BIG});
        shape.setFillColor(fill);
        return shape;
    }
}
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
        const float dashLen = 24.0f, dashStep = 70.0f, edgeGap = 34.0f;  // edgeGap: cách mép ngã tư

    auto addDashEW = [&](float x, float y) {
        sf::RectangleShape line(sf::Vector2f{dashLen, 2.0f});
        line.setFillColor(sf::Color::White);
        line.setPosition(sf::Vector2f{x, y - 1.0f});
        laneLines.push_back(line);
    };
    for (float y : yPositions) {
        for (float x = cx + nsHalfRoad + edgeGap; x < width; x += dashStep)
            addDashEW(x, y);                                   // phía Đông
        for (float x = cx - nsHalfRoad - edgeGap - dashLen; x > -dashLen; x -= dashStep)
            addDashEW(x, y);                                   // phía Tây
    }

    // Vạch nét đứt phân chia chiều đường Bắc - Nam
        auto addDashNS = [&](float y) {
        sf::RectangleShape line(sf::Vector2f{2.0f, dashLen});
        line.setFillColor(sf::Color::Yellow);
        line.setPosition(sf::Vector2f{cx - 1.0f, y});
        laneLines.push_back(line);
    };
    for (float y = cy + ewHalfRoad + edgeGap; y < height; y += dashStep)
        addDashNS(y);                                          // phía Nam
    for (float y = cy - ewHalfRoad - edgeGap - dashLen; y > -dashLen; y -= dashStep)
        addDashNS(y);                                          // phía Bắc

    sf::Color cwColor(220, 220, 220, 200);

    // 5a. Vạch qua đường cắt ngang đường Bắc - Nam (phía trên và dưới ngã tư)
    // Vạch dừng B-N nằm ở cy-101 và cy+101 (mép ngoài); dải vạch qua đường dời ra ngoài thêm 8 px
    // -> cwTop chiếm cy-124..cy-109, cwBottom chiếm cy+109..cy+124
    for (int i = 0; i < 8; ++i) {
    const float x = cx - 53.0f + 14.0f * static_cast<float>(i);
        sf::RectangleShape cwTop(sf::Vector2f{8.0f, 15.0f});
        cwTop.setFillColor(cwColor);
        cwTop.setPosition(sf::Vector2f{x, cy - 113.0f});
        crosswalks.push_back(cwTop);

        sf::RectangleShape cwBottom(sf::Vector2f{8.0f, 15.0f});
        cwBottom.setFillColor(cwColor);
        cwBottom.setPosition(sf::Vector2f{x, cy + 98.0f});
        crosswalks.push_back(cwBottom);
    }

    // 5b. Vạch qua đường cắt ngang đường đôi Đông - Tây (bên trái và phải ngã tư)
    for (int i = 0; i < 11; ++i) {
    const float y = cy - 74.0f + 14.0f * static_cast<float>(i);
        sf::RectangleShape cwLeft(sf::Vector2f{15.0f, 8.0f});
        cwLeft.setFillColor(cwColor);
        cwLeft.setPosition(sf::Vector2f{cx - nsHalfRoad - 18.0f, y});
        crosswalks.push_back(cwLeft);

        sf::RectangleShape cwRight(sf::Vector2f{15.0f, 8.0f});
        cwRight.setFillColor(cwColor);
        cwRight.setPosition(sf::Vector2f{cx + nsHalfRoad + 3.0f, y});
        crosswalks.push_back(cwRight);
    }

    // 6a. Vạch dừng đường đôi Đông - Tây (Cách tâm đúng 60px theo yêu cầu)
    float stopLenEW = ewHalfRoad - ewMedianHalf; 
    sf::RectangleShape stopEast(sf::Vector2f{4.0f, stopLenEW});   // xe đi hướng Đông (nửa dưới)
    stopEast.setFillColor(sf::Color::White);
    stopEast.setPosition(sf::Vector2f{cx - 79.0f, cy + ewMedianHalf});
    stopLines.push_back(stopEast);

    sf::RectangleShape stopWest(sf::Vector2f{4.0f, stopLenEW});   // xe đi hướng Tây (nửa trên)
    stopWest.setFillColor(sf::Color::White);
    stopWest.setPosition(sf::Vector2f{cx + 75.0f, cy - ewHalfRoad}); 
    stopLines.push_back(stopWest);

    // 6b. Vạch dừng đường Bắc - Nam (Cách tâm đúng 101px theo yêu cầu)
    sf::RectangleShape stopSouth(sf::Vector2f{nsHalfRoad, 4.0f});    // xe đi xuống (nửa trái)
    stopSouth.setFillColor(sf::Color::White);
    stopSouth.setPosition(sf::Vector2f{cx - nsHalfRoad, cy - 120.0f});
    stopLines.push_back(stopSouth);

    sf::RectangleShape stopNorth(sf::Vector2f{nsHalfRoad, 4.0f});    // xe đi lên (nửa phải)
    stopNorth.setFillColor(sf::Color::White);
    stopNorth.setPosition(sf::Vector2f{cx, cy + 116.0f});
    stopLines.push_back(stopNorth);
        // 7. Vỉa hè liên tục quanh 4 góc ngã tư (bo tròn), có bó vỉa và viền trắng sát lề đường
    const float curbRadius    = 16.0f;  // bán kính bo góc ở mép đường
    const float curbWidth     = 3.0f;   // bề rộng bó vỉa
    const float sidewalkWidth = 60.0f;  // bề rộng vỉa hè (cộng bó vỉa 3 px = 34 px, vừa đủ chứa hộp đèn)
    const sf::Color curbColor(105, 105, 110);
    const sf::Color sidewalkColor(190, 190, 195);
    const sf::Color groundColor(35, 110, 35);   // trùng màu nền window.clear() trong Renderer.cpp

    struct Corner { float ix, iy, sx, sy; };
    const Corner corners[4] = {
        {cx - nsHalfRoad, cy - ewHalfRoad, -1.0f, -1.0f},   // Tây - Bắc
        {cx + nsHalfRoad, cy - ewHalfRoad, +1.0f, -1.0f},   // Đông - Bắc
        {cx - nsHalfRoad, cy + ewHalfRoad, -1.0f, +1.0f},   // Tây - Nam
        {cx + nsHalfRoad, cy + ewHalfRoad, +1.0f, +1.0f},   // Đông - Nam
    };
    for (const Corner& c : corners) {
        // Lớp 1: bó vỉa, kèm viền trắng 2 px vẽ ra phía lòng đường
        sf::ConvexShape curb = makeBlock(c.ix, c.iy, c.sx, c.sy, 0.0f, curbRadius, curbColor);
        curb.setOutlineThickness(2.0f);
        curb.setOutlineColor(sf::Color(235, 235, 235));
        cornerBlocks.push_back(curb);

        // Lớp 2: mặt vỉa hè
        cornerBlocks.push_back(makeBlock(c.ix, c.iy, c.sx, c.sy, curbWidth, curbRadius, sidewalkColor));

        // Lớp 3: nền bên trong vỉa hè (sau này đặt nhà, cây ở đây)
        cornerBlocks.push_back(makeBlock(c.ix, c.iy, c.sx, c.sy, curbWidth + sidewalkWidth, curbRadius, groundColor));
    }
}

void MapRenderer::draw(sf::RenderWindow& window) {
    window.draw(northSouthRoad);
    window.draw(eastWestRoad);
    window.draw(medianStrip);

    for (const auto& b : cornerBlocks) window.draw(b);
    for (const auto& line : laneLines) window.draw(line);
    for (const auto& cw : crosswalks)  window.draw(cw);
    for (const auto& sl : stopLines)   window.draw(sl);
}