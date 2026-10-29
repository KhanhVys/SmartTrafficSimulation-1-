#include "core/Path.h"

#include <algorithm>

void Path::addPoint(Vec2 point) {
    if (points.empty()) {
        distances.push_back(0.0);
    } else {
        const double segment = length(point - points.back());
        if (segment < 1e-9) {
            return;  // bỏ điểm trùng để không có đoạn dài 0
        }
        distances.push_back(distances.back() + segment);
    }
    points.push_back(point);
}

double Path::getLength() const {
    return distances.empty() ? 0.0 : distances.back();
}

std::size_t Path::findSegment(double s) const {
    if (points.size() < 2) {
        return 0;
    }
    const auto it = std::upper_bound(distances.begin(), distances.end(), s);
    const std::size_t index =
        (it == distances.begin()) ? 0 : static_cast<std::size_t>(it - distances.begin()) - 1;
    return std::min(index, points.size() - 2);
}

Vec2 Path::getPositionAt(double s) const {
    if (points.empty()) {
        return {};
    }
    if (points.size() == 1) {
        return points.front();
    }
    const std::size_t i = findSegment(s);
    return points[i] + getDirectionAt(s) * (s - distances[i]);
}

Vec2 Path::getDirectionAt(double s) const {
    if (points.size() < 2) {
        return {1.0, 0.0};
    }
    const std::size_t i = findSegment(s);
    const Vec2 segment = points[i + 1] - points[i];
    return segment * (1.0 / length(segment));
}
