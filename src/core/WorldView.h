#pragma once

#include "common/Types.h"

class Vehicle;

// Góc nhìn CHỈ ĐỌC mà xe dùng để hỏi thế giới (đèn, xe phía trước).
// Xe không bao giờ sửa đèn hay xe khác qua giao diện này.
// SimulationEngine / CoreSimulation cài đặt giao diện này.
class WorldView {
public:
    virtual ~WorldView() = default;

    // Màu đèn áp dụng cho xe đi vào từ nhánh `arm` với hướng định đi `movement`.
    virtual LightColor getLightColor(Direction arm, Movement movement) const = 0;

    // Khoảng hở (mép đầu xe sau -> mép đuôi xe trước, đơn vị pixel) tới xe ngay phía trước
    // đi cùng lộ trình. Trả false nếu không có xe nào chắn phía trước.
    virtual bool getLeaderGap(const Vehicle& self, double& gap) const = 0;
};
