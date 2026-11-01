#include "MapRenderer.h"
#include <algorithm>
#include <cmath>
#include "core/CoreConstants.h"   
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
        // ---- Độ dày và màu vạch sơn: dùng lại ở mọi chỗ bên dưới ----
    const float paintThin = 2.0f;   // vạch phân làn
    const float paintStop = 4.0f;   // vạch dừng
    const float zebraW    = 8.0f;   // bề rộng 1 sọc qua đường
    const sf::Color paintWhite(238, 238, 232);
    const sf::Color paintYellow(236, 192, 44);

    // ---- Vị trí vạch tính từ STOP_LINE_GAP ----
    const float gap          = static_cast<float>(STOP_LINE_GAP);   // 25
    const float zebraDepth   = 15.0f;                               // chiều sâu vạch qua đường
    const float zebraMargin  = (gap - paintStop - zebraDepth) / 2.0f;   // hở đều 2 phía = 3
    const float dashStart    = gap + 10.0f;                         // vạch đứt đầu tiên cách mép ngã tư
    // 1. Đường Bắc - Nam (rộng 108px)
    northSouthRoad.setSize(sf::Vector2f{nsHalfRoad * 2.0f, height});
    northSouthRoad.setFillColor(sf::Color(50, 50, 50));
    northSouthRoad.setPosition(sf::Vector2f{cx - nsHalfRoad, 0.0f});

    // 2. Đường Đông - Tây (3 làn mỗi chiều + dải phân cách)
    eastWestRoad.setSize(sf::Vector2f{width, ewHalfRoad * 2.0f});
    eastWestRoad.setFillColor(sf::Color(50, 50, 50));
    eastWestRoad.setPosition(sf::Vector2f{0.0f, cy - ewHalfRoad});
    std::mt19937 rng(7);   // seed cố định -> mỗi lần chạy giống nhau
    std::uniform_real_distribution<float> rnd01(0.0f, 1.0f);
    auto addSpecks = [&](float x, float y, float w, float h) {
    const int n = static_cast<int>(w * h / 45.0f);          // ~1 hạt / 45 px²
    for (int i = 0; i < n; ++i) {
        const float s = 1.0f + rnd01(rng) * 1.2f;
        sf::RectangleShape sp(sf::Vector2f{s, s});
        sp.setPosition(sf::Vector2f{x + rnd01(rng) * (w - s), y + rnd01(rng) * (h - s)});
        const float k = rnd01(rng);
        sp.setFillColor(k < 0.4f ? sf::Color(150, 150, 156, 40)
                      : k < 0.8f ? sf::Color(8, 8, 10, 56)
                                 : sf::Color(96, 96, 102, 44));
        roadSpecks.push_back(sp);
    }
};
addSpecks(cx - nsHalfRoad, 0.0f, nsHalfRoad * 2.0f, height);
addSpecks(0.0f, cy - ewHalfRoad, width, ewHalfRoad * 2.0f);
    // 3. Dải phân cách giữa đường Đông - Tây: tách 2 đoạn, KHÔNG chạy xuyên qua ngã tư
    medianLeft.setSize(sf::Vector2f{cx - nsHalfRoad, ewMedianHalf * 2.0f});
    medianLeft.setFillColor(sf::Color(100, 100, 100));
    medianLeft.setPosition(sf::Vector2f{0.0f, cy - ewMedianHalf});

    medianRight.setSize(sf::Vector2f{width - (cx + nsHalfRoad), ewMedianHalf * 2.0f});
    medianRight.setFillColor(sf::Color(100, 100, 100));
    medianRight.setPosition(sf::Vector2f{cx + nsHalfRoad, cy - ewMedianHalf});

    // 4. Vạch kẻ làn đứt nét đường Đông - Tây
    float l1 = ewMedianHalf + ewLaneWidth;         // 35
    float l2 = ewMedianHalf + ewLaneWidth * 2.0f;  // 65
    std::vector<float> yPositions = {cy - l2, cy - l1, cy + l1, cy + l2};
    const float dashLen = 24.0f, dashStep = 70.0f;

    auto addDashEW = [&](float x, float y) {
        sf::RectangleShape line(sf::Vector2f{dashLen, paintThin});
        line.setFillColor(paintWhite);
        line.setPosition(sf::Vector2f{x, y - paintThin / 2.0f});
        laneLines.push_back(line);
    };
    for (float y : yPositions) {
        for (float x = cx + nsHalfRoad + dashStart; x < width; x += dashStep)
            addDashEW(x, y);                                   // phía Đông
        for (float x = cx - nsHalfRoad - dashStart - dashLen; x > -dashLen; x -= dashStep)
            addDashEW(x, y);                                   // phía Tây
    }

    // Vạch nét đứt phân chia chiều đường Bắc - Nam
        auto addDashNS = [&](float y) {
        sf::RectangleShape line(sf::Vector2f{paintThin, dashLen});
        line.setFillColor(paintYellow);
        line.setPosition(sf::Vector2f{cx - paintThin / 2.0f, y});
        laneLines.push_back(line);
    };
    for (float y = cy + ewHalfRoad + dashStart; y < height; y += dashStep)
        addDashNS(y);                                          // phía Nam
    for (float y = cy - ewHalfRoad - dashStart - dashLen; y > -dashLen; y -= dashStep)
        addDashNS(y);                                          // phía Bắc

    sf::Color cwColor(220, 220, 220, 200);

    // 5a. Vạch qua đường cắt ngang đường Bắc - Nam (trên và dưới ngã tư)
    for (int i = 0; i < 8; ++i) {
        const float x = cx - 53.0f + 14.0f * static_cast<float>(i);
        sf::RectangleShape cwTop(sf::Vector2f{zebraW, zebraDepth});
        cwTop.setFillColor(cwColor);
        cwTop.setPosition(sf::Vector2f{x, cy - ewHalfRoad - zebraMargin - zebraDepth});
        crosswalks.push_back(cwTop);

        sf::RectangleShape cwBottom(sf::Vector2f{zebraW, zebraDepth});
        cwBottom.setFillColor(cwColor);
        cwBottom.setPosition(sf::Vector2f{x, cy + ewHalfRoad + zebraMargin});
        crosswalks.push_back(cwBottom);
    }

    // 5b. Vạch qua đường cắt ngang đường đôi Đông - Tây: mỗi nửa đường 6 sọc, chừa dải phân cách
    for (int i = 0; i < 6; ++i) {
        const float d    = 6.0f + 14.0f * static_cast<float>(i);
        const float yTop = cy - ewHalfRoad + d;
        const float yBot = cy + ewMedianHalf + d;
        const float xL   = cx - nsHalfRoad - zebraMargin - zebraDepth;
        const float xR   = cx + nsHalfRoad + zebraMargin;
        for (float y : {yTop, yBot}) {
            sf::RectangleShape l(sf::Vector2f{zebraDepth, zebraW});
            l.setFillColor(cwColor); l.setPosition(sf::Vector2f{xL, y}); crosswalks.push_back(l);
            sf::RectangleShape r(sf::Vector2f{zebraDepth, zebraW});
            r.setFillColor(cwColor); r.setPosition(sf::Vector2f{xR, y}); crosswalks.push_back(r);
        }
    }

       // 6. Vạch dừng: chiếm khoảng cách [gap - paintStop, gap] tính từ mép ngã tư
    const float halfLanes = ewHalfRoad - ewMedianHalf;   // 90

    sf::RectangleShape stopEast(sf::Vector2f{paintStop, halfLanes});
    stopEast.setFillColor(paintWhite);
    stopEast.setPosition(sf::Vector2f{cx - nsHalfRoad - gap, cy + ewMedianHalf});
    stopLines.push_back(stopEast);

    sf::RectangleShape stopWest(sf::Vector2f{paintStop, halfLanes});
    stopWest.setFillColor(paintWhite);
    stopWest.setPosition(sf::Vector2f{cx + nsHalfRoad + gap - paintStop, cy - ewHalfRoad});
    stopLines.push_back(stopWest);

    sf::RectangleShape stopSouth(sf::Vector2f{nsHalfRoad - 2.0f, paintStop});
    stopSouth.setFillColor(paintWhite);
    stopSouth.setPosition(sf::Vector2f{cx - nsHalfRoad, cy - ewHalfRoad - gap});
    stopLines.push_back(stopSouth);

    sf::RectangleShape stopNorth(sf::Vector2f{nsHalfRoad - 2.0f, paintStop});
    stopNorth.setFillColor(paintWhite);
    stopNorth.setPosition(sf::Vector2f{cx + 2.0f, cy + ewHalfRoad + gap - paintStop});
    stopLines.push_back(stopNorth);
    
        // 7. Vỉa hè liên tục quanh 4 góc ngã tư (bo tròn), có bó vỉa và viền trắng sát lề đường
    const float curbRadius    = 16.0f;  // bán kính bo góc ở mép đường
    const float curbWidth     = 3.0f;   // bề rộng bó vỉa
    const float sidewalkWidth = 60.0f;  // bề rộng vỉa hè (cộng bó vỉa 3 px = 34 px, vừa đủ chứa hộp đèn)
    const sf::Color curbColor(105, 105, 110);
    const sf::Color sidewalkColor(200, 200, 204);
    const sf::Color groundColor(158, 154, 144);   

    struct Corner { float ix, iy, sx, sy; };
    const Corner corners[4] = {
        {cx - nsHalfRoad, cy - ewHalfRoad, -1.0f, -1.0f},   // Tây - Bắc
        {cx + nsHalfRoad, cy - ewHalfRoad, +1.0f, -1.0f},   // Đông - Bắc
        {cx - nsHalfRoad, cy + ewHalfRoad, -1.0f, +1.0f},   // Tây - Nam
        {cx + nsHalfRoad, cy + ewHalfRoad, +1.0f, +1.0f},   // Đông - Nam
    };
    for (const Corner& c : corners) {
    auto addRect = [&](float x, float y, float w, float h, sf::Color col) {
    sf::RectangleShape r(sf::Vector2f{w, h});
    r.setPosition(sf::Vector2f{x, y});
    r.setFillColor(col);
    decorRects.push_back(r);
};
auto addCircle = [&](float x, float y, float rad, sf::Color col) {
    sf::CircleShape s(rad);
    s.setOrigin(sf::Vector2f{rad, rad});
    s.setPosition(sf::Vector2f{x, y});
    s.setFillColor(col);
    decorCircles.push_back(s);
};
// Nhà: bóng đổ -> tường (màu mái tối đi một nửa) -> mái
auto addBuilding = [&](float x, float y, float w, float h, sf::Color roof) {
    addRect(x + 4, y + 5, w, h, sf::Color(0, 0, 0, 56));
    addRect(x, y, w, h, sf::Color(roof.r / 2, roof.g / 2, roof.b / 2));
    addRect(x + 2, y + 2, w - 4, h - 4, roof);
};
// Cây: bóng -> tán tối -> tán sáng lệch lên trên
auto addTree = [&](float x, float y, float rad) {
    addCircle(x + rad * 0.3f, y + rad * 0.4f, rad, sf::Color(0, 0, 0, 56));
    addCircle(x, y, rad, sf::Color(40, 92, 52));
    addCircle(x - rad * 0.2f, y - rad * 0.25f, rad * 0.65f, sf::Color(70, 132, 74));
};
// Bụi cây thấp
auto addShrub = [&](float x, float y, float rad) {
    addCircle(x, y, rad, sf::Color(46, 100, 54));
    addCircle(x - rad * 0.3f, y - rad * 0.2f, rad * 0.65f, sf::Color(72, 138, 76));
};
// Lấp góc bằng nhựa đường: quạt tam giác từ góc vuông tới các điểm trên cung
{
    const int   fan = 8;
    const float ax  = c.ix + c.sx * curbRadius;
    const float ay  = c.iy + c.sy * curbRadius;
    for (int i = 0; i < fan; ++i) {
        const float t0 = 1.5707963f * static_cast<float>(i)     / static_cast<float>(fan);
        const float t1 = 1.5707963f * static_cast<float>(i + 1) / static_cast<float>(fan);
        sf::ConvexShape tri;
        tri.setPointCount(3);
        tri.setPoint(0, sf::Vector2f{c.ix, c.iy});
        tri.setPoint(1, sf::Vector2f{ax - c.sx * curbRadius * std::sin(t0), ay - c.sy * curbRadius * std::cos(t0)});
        tri.setPoint(2, sf::Vector2f{ax - c.sx * curbRadius * std::sin(t1), ay - c.sy * curbRadius * std::cos(t1)});
        tri.setFillColor(sf::Color(50, 50, 50));
        cornerBlocks.push_back(tri);
    }
}
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
    
    // ================= CẢNH QUAN 4 GÓC =================
    // Quy ước: (x, y) của addRect/addBuilding là góc TRÊN-TRÁI; của addTree/addShrub/addCircle là TÂM.
    auto addRect = [&](float x, float y, float w, float h, sf::Color col) {
        sf::RectangleShape r(sf::Vector2f{w, h});
        r.setPosition(sf::Vector2f{x, y});
        r.setFillColor(col);
        decorRects.push_back(r);
    };
    auto addCircle = [&](float x, float y, float rad, sf::Color col) {
        sf::CircleShape s(rad);
        s.setOrigin(sf::Vector2f{rad, rad});
        s.setPosition(sf::Vector2f{x, y});
        s.setFillColor(col);
        decorCircles.push_back(s);
    };
    // Nhà: bóng đổ -> tường (màu mái tối đi một nửa) -> mái
    auto addBuilding = [&](float x, float y, float w, float h, sf::Color roof) {
        addRect(x + 4, y + 5, w, h, sf::Color(0, 0, 0, 56));
        addRect(x, y, w, h, sf::Color(roof.r / 2, roof.g / 2, roof.b / 2));
        addRect(x + 2, y + 2, w - 4, h - 4, roof);
    };
    // Cây: bóng -> tán tối -> tán sáng lệch lên trên
    auto addTree = [&](float x, float y, float rad) {
        addCircle(x + rad * 0.3f, y + rad * 0.4f, rad, sf::Color(0, 0, 0, 56));
        addCircle(x, y, rad, sf::Color(40, 92, 52));
        addCircle(x - rad * 0.2f, y - rad * 0.25f, rad * 0.65f, sf::Color(70, 132, 74));
    };
    // Bụi cây thấp
    auto addShrub = [&](float x, float y, float rad) {
        addCircle(x, y, rad, sf::Color(46, 100, 54));
        addCircle(x - rad * 0.3f, y - rad * 0.2f, rad * 0.65f, sf::Color(72, 138, 76));
    };

    const sf::Color lawnColor(96, 142, 86);
    const sf::Color glassColor(158, 204, 226);
    const sf::Color driveColor(186, 184, 178);

    // ---------- GÓC ĐÔNG-BẮC: khu cửa hàng 
    {
        addBuilding(734, 159, 140, 62, sf::Color(188, 172, 150));
        addBuilding(882, 159, 112, 62, sf::Color(160, 176, 194));
        addBuilding(1002, 159, 96, 62, sf::Color(214, 196, 152));
        addBuilding(1106, 159, 80, 62, sf::Color(176, 150, 142));
        // mái hiên sọc đỏ-trắng của cửa hàng thứ nhất
        for (int i = 0; i < 19; ++i)
            addRect(737.0f + 7.0f * i, 212, 7, 9, (i % 2 == 0) ? sf::Color(200, 62, 56) : sf::Color(240, 236, 228));
        // siêu thị phía sau
        addBuilding(734, 36, 212, 112, sf::Color(150, 154, 162));
        addTree(990, 125, 9);
        addTree(1070, 125, 9);
        for (float x = 752; x < 1180; x += 20) addShrub(x, 235, 5);
    }

    // ---------- GÓC TÂY-BẮC: khu nhà ở 
    {
        // hàng trước: sân cỏ, lối xe, nhà
        addRect(10, 127, 152, 115, lawnColor);
        addRect(174, 117, 134, 125, lawnColor);
        addRect(318, 105, 160, 137, lawnColor);
        addRect(112, 213, 24, 29, driveColor);
        addRect(358, 217, 22, 25, driveColor);
        addRect(236, 205, 10, 37, sf::Color(218, 208, 190));
        addBuilding(30, 151, 100, 62, sf::Color(184, 98, 78));
        addBuilding(194, 147, 92, 58, sf::Color(112, 122, 144));
        addBuilding(334, 147, 118, 70, sf::Color(158, 112, 86));
        addTree(296, 170, 14);
        addTree(148, 190, 15);
        for (float x : {42.0f, 64.0f, 86.0f, 206.0f, 262.0f}) addShrub(x, 225, 5);
        // hàng rào cây ngăn 2 dãy nhà
        for (float x = 14; x < 470; x += 9) addShrub(x, 100, 5);
        // hàng sau
        addRect(14, 8, 124, 84, lawnColor);
        addRect(258, 8, 218, 84, lawnColor);
        addBuilding(30, 24, 88, 54, sf::Color(192, 166, 124));
        addBuilding(276, 20, 100, 58, sf::Color(122, 128, 148));
        addBuilding(396, 24, 66, 58, sf::Color(176, 100, 86));
        addTree(176, 50, 24);
        addTree(222, 72, 17);
    }

    // ---------- GÓC TÂY-NAM: khu văn phòng 
    {
        for (float x = 16; x < 470; x += 15) addShrub(x, 566, 5);
        addRect(12, 695, 456, 94, sf::Color(208, 204, 194));          // sân lát
        addBuilding(272, 587, 190, 88, sf::Color(118, 128, 144));
        addBuilding(104, 583, 142, 100, sf::Color(150, 154, 150));
        addBuilding(16, 597, 68, 72, sf::Color(134, 122, 114));
        // mái kính
        addRect(290, 625, 40, 16, glassColor);
        addRect(338, 625, 40, 16, glassColor);
        addRect(386, 625, 40, 16, glassColor);
        addRect(124, 611, 98, 36, glassColor);
        // ghế đá
        for (float x : {344.0f, 204.0f, 64.0f}) addRect(x, 745, 22, 7, sf::Color(128, 90, 60));
        // 6 bồn cây tròn
        for (int i = 0; i < 6; ++i) {
            float x = 422.0f - 72.0f * i;
            addCircle(x, 721, 12, sf::Color(176, 170, 160));
            addCircle(x, 721, 10, sf::Color(92, 68, 50));
            addShrub(x, 721, 7);
        }
        // hàng cây thẳng hàng
        for (int i = 0; i < 7; ++i) addTree(450.0f - 68.0f * i, 767, 11);
    }

    // ---------- GÓC ĐÔNG-NAM: cà phê + chung cư + công viên 
    {
        for (float x = 740; x < 1190; x += 20) addShrub(x, 565, 5);
        // quán cà phê: sân lát, nhà, 3 bàn ô
        addRect(728, 573, 176, 90, sf::Color(214, 204, 186));
        addBuilding(740, 581, 88, 58, sf::Color(172, 102, 84));
        addCircle(858, 597, 10, sf::Color(208, 70, 60));
        addCircle(886, 627, 10, sf::Color(70, 128, 200));
        addCircle(854, 639, 10, sf::Color(232, 190, 60));
        // chung cư
        addBuilding(1014, 585, 172, 96, sf::Color(198, 194, 186));
        addRect(1034, 619, 40, 26, glassColor);
        addRect(1086, 619, 40, 26, glassColor);
        for (float y = 579; y < 680; y += 8) addShrub(1004, y, 4.5f);
        // công viên: cỏ -> lối đi -> hồ
        addRect(728, 679, 282, 108, lawnColor);
        addRect(728, 719, 282, 12, sf::Color(220, 206, 170));
        addRect(856, 679, 12, 108, sf::Color(220, 206, 170));
        addCircle(792, 759, 27, sf::Color(160, 156, 146));
        addCircle(792, 759, 24, sf::Color(112, 164, 194));
        // nhà nhỏ
        addBuilding(1044, 711, 70, 52, sf::Color(112, 144, 124));
        // cây
        addTree(754, 697, 16);
        addTree(826, 697, 13);
        addTree(946, 759, 22);
        addTree(904, 773, 15);
        addTree(982, 705, 14);
        addTree(1146, 745, 20);
        addTree(1026, 777, 12);
    }
}

void MapRenderer::draw(sf::RenderWindow& window) {
    window.draw(northSouthRoad);
    window.draw(eastWestRoad);
    for (const auto& s : roadSpecks) window.draw(s);
    window.draw(medianLeft);
    window.draw(medianRight);   
    
    for (const auto& b : cornerBlocks) window.draw(b);
    for (const auto& r : decorRects)   window.draw(r);
    for (const auto& c : decorCircles) window.draw(c);
    for (const auto& line : laneLines) window.draw(line);
    for (const auto& cw : crosswalks)  window.draw(cw);
    for (const auto& sl : stopLines)   window.draw(sl);
}