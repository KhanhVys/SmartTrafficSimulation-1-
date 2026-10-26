#pragma once

// Các kiểu dùng chung cho TOÀN BỘ project (theo tài liệu giao tiếp).
// Mọi module chỉ #include file này, KHÔNG tự định nghĩa lại các enum bên dưới.

enum class Direction { North, South, East, West };
enum class Movement  { Straight, Left, Right, UTurn };
enum class LightColor { Red, Yellow, Green };

// Struct Point dùng để trả về tọa độ (do phần Core yêu cầu)
struct Point {
    float x;
    float y;
};