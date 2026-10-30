#pragma once

#include <memory>
#include <vector>

#include "common/Constants.h"
#include "common/Types.h"
#include "core/CoreConstants.h"
#include "core/RoadNetwork.h"
#include "core/TrafficLight.h"
#include "core/Vehicle.h"
#include "core/WorldView.h"

// Lõi mô phỏng chạy ĐỘC LẬP, một luồng, không khoá, không phụ thuộc SFML.
// Người 3 bọc lớp này trong luồng mô phỏng và tự thêm khoá/snapshot;
// Người 5 đọc dữ liệu qua getter (hoặc qua snapshot do Người 3 sao chép).
//
// Thứ tự mỗi tick đúng tài liệu giao tiếp:
//   advanceClock() -> updateTrafficLight() -> updateVehicles()
// Tách riêng từng bước để bên gọi khoá lightMutex trước, vehicleMutex sau.
// step() = gọi cả ba bước liên tiếp (dùng khi chạy một luồng / chạy test).
class CoreSimulation : public WorldView {
public:
    explicit CoreSimulation(int vehicleCount = NUM_VEHICLES, double maxSpeed = DEFAULT_SPEED);

    void advanceClock();
    void updateTrafficLight();
    void updateVehicles();
    void step();

    long long getCurrentTick() const;
    double getSimulationSeconds() const;
    bool allVehiclesFinished() const;

    const std::vector<std::unique_ptr<Vehicle>>& getVehicles() const;
    const TrafficLight& getTrafficLight() const;
    const RoadNetwork& getRoadNetwork() const;

    // Tốc độ xe tuỳ chỉnh được (pixel/giây), áp dụng cho mọi xe.
    void setMaxSpeed(double maxSpeed);

    // WorldView
    LightColor getLightColor(Direction arm, Movement movement) const override;
    bool getLeaderGap(const Vehicle& self, double& gap) const override;

private:
    RoadNetwork network;
    TrafficLight trafficLight;
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    long long currentTick = 0;
};
