#pragma once

#include <string>

#include "common/Types.h"
#include "core/IRoadComponent.h"

// Một nhánh đường nối vào ngã tư (gồm chiều vào và chiều ra của nhánh đó).
// lane 0 là làn sát tim đường / dải phân cách, lane lớn dần ra phía lề phải.
class Road : public IRoadComponent {
public:
    Road(Direction arm, int laneCount, double laneWidth, double centerHalfGap, double length);

    std::string getName() const override;
    int getCapacity() const override;

    Direction getArm() const;
    int getLaneCount() const;
    double getLaneWidth() const;
    double getLength() const;     // từ mép ngã tư ra tới rìa thế giới
    double getHalfWidth() const;  // nửa bề rộng của cả mặt đường (hai chiều)
    // Khoảng cách từ tim đường tới tim làn, đo về phía lề phải của chiều xe chạy.
    double getLaneCenterOffset(int lane) const;

private:
    Direction arm;
    int laneCount;
    double laneWidth;
    double centerHalfGap;
    double length;
};
