#pragma once

enum class TrafficPhase
{
    NorthSouthStraightRight,
    NorthSouthLeftUTurn,
    EastWestStraightRight,
    EastWestLeftUTurn,
    AllRed
};

enum class SignalState
{
    Red,
    Yellow,
    Green
};

class TrafficController
{
public:
    static constexpr int TICKS_PER_SECOND = 50;
    static constexpr int CYCLE_LENGTH_SECONDS = 38;
    static constexpr int CYCLE_LENGTH_TICKS =
        TICKS_PER_SECOND * CYCLE_LENGTH_SECONDS;

    TrafficController();

    void update();

    int getCurrentTick() const;
    int getCurrentSecond() const;

    TrafficPhase getCurrentPhase() const;
    SignalState getSignalState(TrafficPhase phase) const;

private:
    int currentTick;
};