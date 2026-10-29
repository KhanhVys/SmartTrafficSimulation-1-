#pragma once

#include <cstddef>
#include <vector>

#include "core/Vec2.h"

// Đường đi của một xe: dãy điểm nối thẳng, tra vị trí theo quãng đường đã đi (s).
// s nằm ngoài [0, độ dài] thì ngoại suy thẳng theo đoạn đầu/cuối.
class Path {
public:
    void addPoint(Vec2 point);
    double getLength() const;
    Vec2 getPositionAt(double s) const;
    Vec2 getDirectionAt(double s) const;  // vector đơn vị

private:
    std::size_t findSegment(double s) const;

    std::vector<Vec2> points;
    std::vector<double> distances;  // khoảng cách tích luỹ tới từng điểm
};
