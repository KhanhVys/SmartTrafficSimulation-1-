#pragma once

#include <memory>

class Vehicle;
class WorldView;

// State Pattern cho xe: mỗi state tự quyết định xe di chuyển ra sao trong tick này
// và trả về state kế tiếp (nullptr = giữ nguyên). Vehicle mới là nơi thay state,
// nhờ vậy state không bị huỷ khi đang chạy chính hàm của nó.
class IVehicleState {
public:
    virtual ~IVehicleState() = default;

    virtual const char* getName() const = 0;
    virtual std::unique_ptr<IVehicleState> update(Vehicle& vehicle, double dt, const WorldView& view) = 0;
};
