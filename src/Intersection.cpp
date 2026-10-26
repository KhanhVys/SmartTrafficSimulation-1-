// Intersection.cpp
#include "Intersection.h"
float Intersection::getDistanceToStopLine(float x, float y, Direction dir) const {
    const float IX = 400.0f; const float IY = 300.0f; const float OFFSET = 50.0f; 
    if (roads.empty()) return 9999.0f;
    switch (dir) {
        case Direction::South: return (IY - OFFSET) - y;
        case Direction::North: return y - (IY + OFFSET);
        case Direction::East:  return (IX - OFFSET) - x;
        case Direction::West:  return x - (IX + OFFSET);
    }
    return 1000.0f;
}