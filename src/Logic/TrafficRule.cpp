#include "TrafficRule.h"

TrafficPhase TrafficRule::getRequiredPhase(
    Direction direction,
    MovementType movement)
{
    bool isNorthSouth =
        direction == Direction::North ||
        direction == Direction::South;

    bool isStraightOrRight =
        movement == MovementType::Straight ||
        movement == MovementType::Right;

    if (isNorthSouth)
    {
        if (isStraightOrRight)
        {
            return TrafficPhase::NorthSouthStraightRight;
        }

        return TrafficPhase::NorthSouthLeftUTurn;
    }

    if (isStraightOrRight)
    {
        return TrafficPhase::EastWestStraightRight;
    }

    return TrafficPhase::EastWestLeftUTurn;
}

LaneType TrafficRule::getLaneType(
    Direction direction,
    MovementType movement)
{
    bool isNorthSouth =
        direction == Direction::North ||
        direction == Direction::South;

    if (isNorthSouth)
    {
        if (movement == MovementType::Left ||
            movement == MovementType::UTurn)
        {
            return LaneType::LeftUTurn;
        }

        return LaneType::StraightRight;
    }

    if (movement == MovementType::Left ||
        movement == MovementType::UTurn)
    {
        return LaneType::LeftUTurn;
    }

    if (movement == MovementType::Right)
    {
        return LaneType::Right;
    }

    return LaneType::Straight;
}

bool TrafficRule::canMove(
    Direction direction,
    MovementType movement,
    const TrafficController& controller)
{
    TrafficPhase requiredPhase =
        getRequiredPhase(direction, movement);

    return controller.getSignalState(requiredPhase)
        == SignalState::Green;
}

bool TrafficRule::shouldStop(
    Direction direction,
    MovementType movement,
    const TrafficController& controller,
    bool hasCrossedStopLine)
{
    // Xe đã vượt vạch dừng phải tiếp tục qua giao lộ.
    if (hasCrossedStopLine)
    {
        return false;
    }

    // Xe chỉ được bắt đầu đi khi đèn của hướng đi xanh.
    return !canMove(direction, movement, controller);
}

bool TrafficRule::shouldSlowDown(
    Direction direction,
    MovementType movement,
    const TrafficController& controller,
    bool hasCrossedStopLine)
{
    if (hasCrossedStopLine)
    {
        return false;
    }

    TrafficPhase requiredPhase =
        getRequiredPhase(direction, movement);

    return controller.getSignalState(requiredPhase)
        == SignalState::Yellow;
}