#include "core/Road.h"

#include "common/Types.h"
#include "core/CoreConstants.h"

Road::Road(Direction arm, int laneCount, double laneWidth, double centerHalfGap, double length)
    : arm(arm),
      laneCount(laneCount),
      laneWidth(laneWidth),
      centerHalfGap(centerHalfGap),
      length(length) {}

std::string Road::getName() const {
    switch (arm) {
        case Direction::North: return "North road";
        case Direction::South: return "South road";
        case Direction::East: return "East road";
        case Direction::West: return "West road";
    }
    return "Road";
}

int Road::getCapacity() const {
    return laneCount * static_cast<int>(length / (CAR_LENGTH + MIN_FOLLOW_GAP));
}

Direction Road::getArm() const { return arm; }
int Road::getLaneCount() const { return laneCount; }
double Road::getLaneWidth() const { return laneWidth; }
double Road::getLength() const { return length; }

double Road::getHalfWidth() const { return centerHalfGap + laneCount * laneWidth; }

double Road::getLaneCenterOffset(int lane) const {
    return centerHalfGap + laneWidth * (static_cast<double>(lane) + 0.5);
}
