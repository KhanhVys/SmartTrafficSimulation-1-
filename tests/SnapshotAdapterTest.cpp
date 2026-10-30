#undef NDEBUG  // giữ assert hoạt động cả khi build Release

#include "integration/SnapshotAdapter.h"
#include "core/CoreSimulation.h"
#include <cassert>
#include <set>
#include <iostream>

void testSnapshotAdapter() {
    std::printf("[SnapshotAdapterTest] Dang kiem tra SnapshotAdapter...\n");

    // Khởi tạo mô phỏng với số lượng xe mặc định
    CoreSimulation sim;
    
    // 1. Kiểm tra số lượng đèn giao thông (phải đúng 8 đèn: 4 hướng x 2 loại đèn)
    auto lights = SnapshotAdapter::lightsOf(sim);
    assert(lights.size() == 8);
    std::printf("  -> OK: So luong den dung (8 den).\n");

    // Chạy vài bước để giả lập trạng thái xe di chuyển
    for (int i = 0; i < 5; ++i) {
        sim.step();
    }

    // 2. Kiểm tra tính duy nhất của vehicleId (không bị trùng ID)
    auto vehicles = SnapshotAdapter::vehiclesOf(sim);
    std::set<int> uniqueIds;
    for (const auto& v : vehicles) {
        // Đảm bảo không có ID nào bị trùng trong tập hợp
        assert(uniqueIds.find(v.vehicleId) == uniqueIds.end());
        uniqueIds.insert(v.vehicleId);
    }
    std::printf("  -> OK: Khong co xe nao bi trung vehicleId (%zu xe).\n", vehicles.size());

// 3. Kiểm tra SnapshotAdapter đã loại bỏ xe kết thúc
bool hasFinishedVehicle = false;

// Chạy mô phỏng cho đến khi có ít nhất một xe kết thúc
for (int i = 0; i < 100000 && !hasFinishedVehicle; ++i) {
    sim.step();

    for (const auto& vehicle : sim.getVehicles()) {
        if (vehicle->isFinished()) {
            hasFinishedVehicle = true;
            break;
        }
    }
}

assert(hasFinishedVehicle);

// Lấy snapshot sau khi đã có xe kết thúc
auto vehiclesAfterFinish = SnapshotAdapter::vehiclesOf(sim);

// Kiểm tra không có xe đã kết thúc trong snapshot
for (const auto& vehicle : sim.getVehicles()) {
    if (vehicle->isFinished()) {
        for (const auto& snapshot : vehiclesAfterFinish) {
            assert(snapshot.vehicleId != vehicle->getId());
        }
    }
}

std::printf("  -> OK: Da loai bo xe isFinished().\n");
}

int main() {
    try {
        testSnapshotAdapter();
        std::printf("TAT CA TEST SNAPSHOT ADAPTER DAT\n");
        return 0;
    } catch (const std::exception& e) {
        std::printf("LOI TEST: %s\n", e.what());
        return 1;
    }
}