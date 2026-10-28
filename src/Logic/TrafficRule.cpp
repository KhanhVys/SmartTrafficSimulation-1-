
#include "TrafficRule.h"

TrafficPhase TrafficRule::getRequiredPhase(
    Direction direction,
    Movement movement)
{
    bool isNorthSouth =
        (direction == Direction::North ||
         direction == Direction::South);

    if (isNorthSouth)
    {
        if (movement == Movement::Left ||
            movement == Movement::UTurn)
        {
            return TrafficPhase::NorthSouthLeftUTurn;
        }

        return TrafficPhase::NorthSouthStraightRight;
    }

    if (movement == Movement::Left ||
        movement == Movement::UTurn)
    {
        return TrafficPhase::EastWestLeftUTurn;
    }

    return TrafficPhase::EastWestStraightRight;
}

LaneType TrafficRule::getLaneType(
    Direction direction,
    Movement movement)
{
    bool isNorthSouth =
        (direction == Direction::North ||
         direction == Direction::South);

    if (movement == Movement::Left ||
        movement == Movement::UTurn)
    {
        return LaneType::LeftUTurn;
    }

    if (isNorthSouth)
    {
        return LaneType::StraightRight;
    }

    if (movement == Movement::Right)
    {
        return LaneType::Right;
    }

    return LaneType::Straight;
}

bool TrafficRule::canMove(
    Direction direction,
    Movement movement,
    const TrafficController& controller)
{
    TrafficPhase phase = getRequiredPhase(direction, movement);

    return controller.getSignalState(phase) == LightColor::Green;
}

bool TrafficRule::shouldStop(
    Direction direction,
    Movement movement,
    const TrafficController& controller,
    bool hasCrossedStopLine)
{
    if (hasCrossedStopLine)
    {
        return false;
    }

    return !canMove(direction, movement, controller);
}

bool TrafficRule::shouldSlowDown(
    Direction direction,
    Movement movement,
    const TrafficController& controller,
    bool hasCrossedStopLine)
{
    if (hasCrossedStopLine)
    {
        return false;
    }

    TrafficPhase phase = getRequiredPhase(direction, movement);

    return controller.getSignalState(phase) == LightColor::Yellow;
}

bool TrafficRule::shouldStop(
    LightColor lightColor,
    bool hasCrossedStopLine)
{
    if (hasCrossedStopLine)
    {
        return false;
    }

    return lightColor == LightColor::Yellow ||
           lightColor == LightColor::Red;
}
