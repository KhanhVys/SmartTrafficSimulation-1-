#pragma once

constexpr int TICKS_PER_SECOND = 50;
constexpr double TICK_SECONDS = 0.02;
constexpr int CYCLE_TICKS = 38 * TICKS_PER_SECOND;

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

constexpr float DEFAULT_MAX_SPEED = 100.0f;
constexpr float DEFAULT_ACCELERATION = 50.0f;
constexpr float DEFAULT_DECELERATION = 250.0f;
constexpr float DEFAULT_SAFE_DISTANCE = 40.0f;

// Fix vấn đề 12: Đưa map bounds vào Constants
constexpr float MAP_MIN_X = -200.0f;
constexpr float MAP_MAX_X = 1000.0f;
constexpr float MAP_MIN_Y = -200.0f;
constexpr float MAP_MAX_Y = 800.0f;

constexpr float TURN_RADIUS = 80.0f;
constexpr int NUM_VEHICLES = 13;