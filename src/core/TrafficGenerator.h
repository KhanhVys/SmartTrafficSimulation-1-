#pragma once

#include <memory>
#include <vector>

#include "common/Types.h"
#include "core/RoadNetwork.h"
#include "core/Vehicle.h"

// Sinh xe MỘT LẦN lúc khởi động (13 xe, không sinh thêm).
class TrafficGenerator {
public:
    TrafficGenerator(const RoadNetwork& network, double maxSpeed);

    // Sinh một xe, đầu xe cách vạch dừng `distanceToStopLine` pixel.
    std::unique_ptr<Vehicle> generateVehicle(int vehicleId, Direction arm, int lane, Movement movement,
                                             double distanceToStopLine) const;

    // Kịch bản demo cố định (lặp lại được): phủ đủ 4 nhánh, 4 hướng đi, có xe xếp hàng cùng làn.
    // count nằm trong [0, 13], ngoài khoảng đó ném std::invalid_argument.
    std::vector<std::unique_ptr<Vehicle>> generateInitialVehicles(int count) const;

    // Phân bổ NGẪU NHIÊN: mỗi lần chạy mỗi khác. Chỉ bốc tổ hợp (nhánh, làn, hướng) hợp lệ,
    // và không nhét quá số xe mà một làn chứa được. seed = 0 nghĩa là lấy ngẫu nhiên thật.
    std::vector<std::unique_ptr<Vehicle>> generateRandomVehicles(int count, unsigned seed = 0) const;
private:
    const RoadNetwork& network;
    double maxSpeed;
};
