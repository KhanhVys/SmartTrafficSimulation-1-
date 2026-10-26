#include "Road.h"

float Intersection::getDistanceToStopLine(float x, float y, Direction dir) const {
    const float INTERSECTION_X = 400.0f;
    const float INTERSECTION_Y = 300.0f;
    const float OFFSET = 50.0f; 
    
    switch (dir) {
        case Direction::South: return (INTERSECTION_Y - OFFSET) - y;
        case Direction::North: return y - (INTERSECTION_Y + OFFSET);
        case Direction::East:  return (INTERSECTION_X - OFFSET) - x;
        case Direction::West:  return x - (INTERSECTION_X + OFFSET);
    }
    return 1000.0f;
}