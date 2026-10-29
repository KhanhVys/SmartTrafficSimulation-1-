#include "core/Intersection.h"

#include "core/CoreConstants.h"

Intersection::Intersection(double halfWidth, double halfHeight)
    : halfWidth(halfWidth), halfHeight(halfHeight) {}

std::string Intersection::getName() const { return "Intersection"; }
int Intersection::getCapacity() const { return INTERSECTION_CAPACITY; }
double Intersection::getHalfWidth() const { return halfWidth; }
double Intersection::getHalfHeight() const { return halfHeight; }
