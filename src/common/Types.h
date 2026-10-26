#pragma once

// Hướng di chuyển
enum class Direction { North, South, East, West };

// Hành động tại giao lộ (Đã thống nhất tên với Người 2)
enum class Movement  { Straight, Left, Right, UTurn };

// Màu đèn
enum class LightColor { Red, Yellow, Green };

namespace Config {
    const int   TICKS_PER_SECOND        = 50;
    const int   LIGHT_CYCLE_DURATION    = 38;
    const int   TOTAL_TICKS_PER_CYCLE   = LIGHT_CYCLE_DURATION * TICKS_PER_SECOND;

    // Đơn vị: pixel/giây
    const float DEFAULT_MAX_SPEED       = 100.0f;
    const float DEFAULT_ACCELERATION    = 50.0f;
    const float DEFAULT_BRAKING         = 250.0f;
    const float DEFAULT_SAFE_DISTANCE   = 40.0f;
}