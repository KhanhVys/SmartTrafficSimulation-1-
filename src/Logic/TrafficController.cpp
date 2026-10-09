#include "TrafficController.h"

TrafficController::TrafficController()
    : currentTick(0)
{
}

void TrafficController::update()
{
    currentTick++;

    if (currentTick >= CYCLE_LENGTH_TICKS)
    {
        currentTick = 0;
    }
}

int TrafficController::getCurrentTick() const
{
    return currentTick;
}

int TrafficController::getCurrentSecond() const
{
    return currentTick / TICKS_PER_SECOND;
}

TrafficPhase TrafficController::getCurrentPhase() const
{
    int second = getCurrentSecond();

    if (second >= 0 && second <= 7)
    {
        return TrafficPhase::NorthSouthStraightRight;
    }

    if (second >= 8 && second <= 9)
    {
        return TrafficPhase::AllRed;
    }

    if (second >= 10 && second <= 16)
    {
        return TrafficPhase::NorthSouthLeftUTurn;
    }

    if (second >= 17 && second <= 18)
    {
        return TrafficPhase::AllRed;
    }

    if (second >= 19 && second <= 26)
    {
        return TrafficPhase::EastWestStraightRight;
    }

    if (second >= 27 && second <= 28)
    {
        return TrafficPhase::AllRed;
    }

    if (second >= 29 && second <= 35)
    {
        return TrafficPhase::EastWestLeftUTurn;
    }

    return TrafficPhase::AllRed;
}

SignalState TrafficController::getSignalState(
    TrafficPhase phase) const
{
    int second = getCurrentSecond();

    switch (phase)
    {
    case TrafficPhase::NorthSouthStraightRight:
        if (second >= 0 && second <= 5)
        {
            return SignalState::Green;
        }

        if (second >= 6 && second <= 7)
        {
            return SignalState::Yellow;
        }

        return SignalState::Red;

    case TrafficPhase::NorthSouthLeftUTurn:
        if (second >= 10 && second <= 14)
        {
            return SignalState::Green;
        }

        if (second >= 15 && second <= 16)
        {
            return SignalState::Yellow;
        }

        return SignalState::Red;

    case TrafficPhase::EastWestStraightRight:
        if (second >= 19 && second <= 24)
        {
            return SignalState::Green;
        }

        if (second >= 25 && second <= 26)
        {
            return SignalState::Yellow;
        }

        return SignalState::Red;

    case TrafficPhase::EastWestLeftUTurn:
        if (second >= 29 && second <= 33)
        {
            return SignalState::Green;
        }

        if (second >= 34 && second <= 35)
        {
            return SignalState::Yellow;
        }

        return SignalState::Red;

    case TrafficPhase::AllRed:
        return SignalState::Red;
    }

    return SignalState::Red;
}