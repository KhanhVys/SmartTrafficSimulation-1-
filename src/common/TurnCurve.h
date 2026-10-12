#pragma once

#include <algorithm>
#include <cmath>

struct Point2D {
    double x = 0.0;
    double y = 0.0;
};

struct TurnCurveSample {
    Point2D position;
    double rotationAngleDegrees = 0.0;
};

class TurnCurve {
public:
    // Quadratic Bézier: P(t) = (1-t)^2 P0 + 2(1-t)t P1 + t^2 P2.
    // P1 điều khiển độ cong; t chạy từ 0.0 đến 1.0.
    static TurnCurveSample sampleQuadraticBezier(
        Point2D start,
        Point2D control,
        Point2D end,
        double progress
    ) {
        const double t = std::clamp(progress, 0.0, 1.0);
        const double oneMinusT = 1.0 - t;

        const Point2D position{
            oneMinusT * oneMinusT * start.x
                + 2.0 * oneMinusT * t * control.x
                + t * t * end.x,
            oneMinusT * oneMinusT * start.y
                + 2.0 * oneMinusT * t * control.y
                + t * t * end.y
        };

        // Đạo hàm Bézier là hướng tiếp tuyến của xe trên đường cong.
        const double tangentX =
            2.0 * oneMinusT * (control.x - start.x)
            + 2.0 * t * (end.x - control.x);
        const double tangentY =
            2.0 * oneMinusT * (control.y - start.y)
            + 2.0 * t * (end.y - control.y);

        constexpr double PI = 3.14159265358979323846;
        const double rotationAngleDegrees =
            std::atan2(tangentY, tangentX) * 180.0 / PI;

        return {position, rotationAngleDegrees};
    }
};
