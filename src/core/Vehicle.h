#pragma once

#include <memory>

#include "common/Types.h"
#include "core/IVehicleState.h"
#include "core/Route.h"
#include "core/Vec2.h"

class WorldView;

// Xe ô tô. Chạy dọc một Route cố định (không chuyển làn).
// Vị trí của xe là quãng đường s của ĐẦU XE dọc Route (frontPosition).
class Vehicle {
public:
    Vehicle(int vehicleId, Direction direction, int lane, Movement movement, Route route,
            double startFrontPosition, double maxSpeed);

    // Gọi mỗi tick bởi luồng mô phỏng. dt tính bằng giây (TICK_SECONDS).
    void update(double dt, const WorldView& view);

    // ---- Dữ liệu để vẽ / chụp snapshot (chỉ đọc) ----
    int getId() const;
    Direction getDirection() const;  // nhánh đường xe đi vào
    int getLane() const;
    Movement getMovement() const;
    const Route& getRoute() const;
    double getFrontPosition() const;
    double getSpeed() const;
    double getMaxSpeed() const;
    Vec2 getPosition() const;          // tâm xe
    double getHeadingDegrees() const;  // 0 = hướng +x, tăng theo chiều kim đồng hồ (như SFML)
    const char* getStateName() const;
    bool isSignalingLeft() const;      // xi-nhan trái: rẽ trái và quay đầu
    bool isFinished() const;           // đã ra khỏi bản đồ

    void setMaxSpeed(double newMaxSpeed);

    // ---- Dành cho các State (xem IVehicleState) ----
    double getTargetSpeed() const;
    bool isInTurn() const;
    double getBrakingDistance() const;  // quãng phanh với gia tốc phanh bình thường
    // Khoảng cách tới điểm phải dừng gần nhất: vạch dừng (khi TrafficRule báo phải dừng)
    // hoặc điểm cách đuôi xe phía trước MIN_FOLLOW_GAP. Trả false nếu không có vật cản.
    bool getObstacleDistance(const WorldView& view, double& distance) const;
    void changeSpeed(double targetSpeed, double dt);
    // Phanh để dừng sau đúng `distance` pixel. Trả true khi đã dừng hẳn.
    bool brakeToStop(double distance, double dt);
    void halt();

private:
    int vehicleId;
    Direction direction;
    int lane;
    Movement movement;
    Route route;
    double frontPosition;
    double speed;
    double maxSpeed;
    bool finished = false;
    std::unique_ptr<IVehicleState> state;
};
