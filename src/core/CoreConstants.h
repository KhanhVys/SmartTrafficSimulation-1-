#pragma once

// Hằng số hình học và vật lý của Core Simulation (Người 1).
// Đơn vị: pixel (độ dài), giây (thời gian). Toạ độ thế giới trùng toạ độ
// cửa sổ SFML 1200x800, tâm ngã tư ở giữa cửa sổ. Trục y hướng xuống.
// Thời gian đèn nằm ở common/Constants.h (do nhóm trưởng chốt), không lặp lại ở đây.

constexpr double WORLD_WIDTH = 1200.0;
constexpr double WORLD_HEIGHT = 800.0;
constexpr double WORLD_CENTER_X = WORLD_WIDTH / 2.0;
constexpr double WORLD_CENTER_Y = WORLD_HEIGHT / 2.0;

// Đường đôi (Đông-Tây): mỗi chiều 3 làn, có dải phân cách ở giữa.
constexpr int EW_LANE_COUNT = 3;
constexpr double EW_LANE_WIDTH = 30.0;
constexpr double EW_MEDIAN_HALF_WIDTH = 5.0;
constexpr double EW_ROAD_LENGTH = 540.0;  // từ mép ngã tư ra tới rìa cửa sổ

// Đường đơn (Bắc-Nam): mỗi chiều 1 làn đủ rộng cho 2 xe đứng song song.
// Trong code gọi hai hàng xe đó là "lane" 0 và 1 (0 = hàng sát tim đường).
constexpr int NS_LANE_COUNT = 2;
constexpr double NS_LANE_WIDTH = 26.0;
constexpr double NS_CENTER_HALF_GAP = 2.0;
constexpr double NS_ROAD_LENGTH = 300.0;

constexpr int INTERSECTION_CAPACITY = 8;

// Xe
constexpr double CAR_LENGTH = 40.0;
constexpr double CAR_WIDTH = 20.0;
constexpr double DEFAULT_SPEED = 90.0;           // pixel/giây, chỉnh được qua setMaxSpeed()
constexpr double ACCELERATION = 60.0;            // pixel/giây^2
constexpr double COMFORT_DECELERATION = 90.0;    // phanh bình thường
constexpr double HARD_DECELERATION = 180.0;      // phanh gấp tối đa
constexpr double MIN_FOLLOW_GAP = 12.0;          // khoảng hở tối thiểu giữa hai xe cùng làn
constexpr double STOP_LINE_GAP = 29.0;           // vạch dừng cách mép ngã tư            // vạch dừng cách mép ngã tư
constexpr double TURN_SPEED_RATIO = 0.5;         // tốc độ khi rẽ = tỉ lệ này * tốc độ tối đa
constexpr double TURN_SLOWDOWN_DISTANCE = 70.0;  // bắt đầu giảm tốc trước vạch dừng khi sắp rẽ
constexpr double WAIT_TOLERANCE = 3.0;           // sai số coi là "đã tới vạch / sát xe trước"
constexpr double STOP_SPEED_EPSILON = 1.0;
constexpr double SPEED_EPSILON = 0.5;
constexpr double LINE_CROSS_TOLERANCE = 0.5;     // vượt vạch quá mức này mới tính là "đã qua vạch"
constexpr double BRAKE_RELEASE_RATIO = 0.6;      // nhả phanh khi quãng phanh < 60% khoảng cách

constexpr int BEZIER_SEGMENTS = 32;              // số đoạn xấp xỉ đường cong rẽ
