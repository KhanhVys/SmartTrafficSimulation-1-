#pragma once

#include "TrafficController.h"

enum class Direction
{
    North,
    South,
    East,
    West
};

enum class MovementType
{
    Straight,
    Left,
    Right,
    UTurn
};

enum class LaneType
{
    LeftUTurn,
    Straight,
    Right,
    StraightRight
};

class TrafficRule
{
public:
    static TrafficPhase getRequiredPhase(
        Direction direction,
        MovementType movement);

    static LaneType getLaneType(
        Direction direction,
        MovementType movement);

    static bool canMove(
        Direction direction,
        MovementType movement,
        const TrafficController& controller);

    static bool shouldStop(
        Direction direction,
        MovementType movement,
        const TrafficController& controller,
        bool hasCrossedStopLine);

    static bool shouldSlowDown(
        Direction direction,
        MovementType movement,
        const TrafficController& controller,
        bool hasCrossedStopLine);
};