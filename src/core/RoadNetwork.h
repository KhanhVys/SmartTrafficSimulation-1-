#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "common/Types.h"
#include "core/Intersection.h"
#include "core/IRoadComponent.h"
#include "core/Road.h"
#include "core/Route.h"

// Mạng đường (nút ghép của Composite). Chỉ ghi lúc khởi tạo, sau đó bất biến,
// nên mọi luồng đều đọc được mà không cần khoá.
class RoadNetwork : public IRoadComponent {
public:
    // 1 ngã tư + 4 nhánh: đường đôi Đông-Tây (3 làn/chiều), đường đơn Bắc-Nam (1 làn 2 hàng/chiều).
    static RoadNetwork createDefault();

    std::string getName() const override;
    int getCapacity() const override;
    void addComponent(std::shared_ptr<IRoadComponent> component) override;
    std::vector<std::shared_ptr<IRoadComponent>> getConnectedComponents() const override;

    const Road& getRoad(Direction arm) const;
    const Intersection& getIntersection() const;

    // Làn trái = rẽ trái + quay đầu; Đông-Tây: làn giữa đi thẳng, làn phải rẽ phải;
    // Bắc-Nam: hàng ngoài = đi thẳng + rẽ phải.
    bool isMovementAllowed(Direction arm, int lane, Movement movement) const;

    // Dựng lộ trình cho xe vào từ nhánh `arm`, đi làn `lane`, định đi `movement`.
    // Ném std::invalid_argument nếu tổ hợp không hợp lệ.
    Route buildRoute(Direction arm, int lane, Movement movement) const;

private:
    std::vector<std::shared_ptr<IRoadComponent>> components;
    std::array<std::shared_ptr<Road>, 4> roads;
    std::shared_ptr<Intersection> intersection;
};
