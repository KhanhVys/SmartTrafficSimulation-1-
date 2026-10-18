#pragma once

// Shared types used across the project.

// Direction from which a vehicle enters the intersection.
enum class Direction { North, South, East, West };

// Direction in which a vehicle intends to travel.
enum class Movement { Straight, Left, Right, UTurn };

// Traffic light color.
enum class LightColor { Red, Yellow, Green };
