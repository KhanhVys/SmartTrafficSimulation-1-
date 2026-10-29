#pragma once

// Thời gian tính bằng tick (số nguyên). TICK_SECONDS chỉ dùng khi tính di chuyển.
constexpr int TICKS_PER_SECOND = 50;
constexpr double TICK_SECONDS = 0.02;

constexpr int NUM_VEHICLES = 13;

constexpr int STRAIGHT_GREEN_TICKS = 6 * TICKS_PER_SECOND;
constexpr int TURN_GREEN_TICKS = 5 * TICKS_PER_SECOND;
constexpr int YELLOW_TICKS = 2 * TICKS_PER_SECOND;
constexpr int ALL_RED_TICKS = 2 * TICKS_PER_SECOND;

constexpr int STRAIGHT_PHASE_TICKS = STRAIGHT_GREEN_TICKS + YELLOW_TICKS + ALL_RED_TICKS;
constexpr int TURN_PHASE_TICKS = TURN_GREEN_TICKS + YELLOW_TICKS + ALL_RED_TICKS;

constexpr int NS_STRAIGHT_START = 0;
constexpr int NS_TURN_START = NS_STRAIGHT_START + STRAIGHT_PHASE_TICKS;
constexpr int EW_STRAIGHT_START = NS_TURN_START + TURN_PHASE_TICKS;
constexpr int EW_TURN_START = EW_STRAIGHT_START + STRAIGHT_PHASE_TICKS;

constexpr int CYCLE_TICKS = EW_TURN_START + TURN_PHASE_TICKS;

static_assert(CYCLE_TICKS == 38 * TICKS_PER_SECOND, "Chu ky den phai la 38 giay");