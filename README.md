# SmartTrafficSimulation - Concurrency Module

Đây là module của Người 3 - Concurrency / Thread trong dự án mô phỏng giao thông đô thị thời gian thực.

## Chức năng

Module cung cấp:

- `SimulationThread`: quản lý luồng mô phỏng.
- `SnapshotStore`: đồng bộ dữ liệu giữa luồng mô phỏng và luồng GUI.
- `SimulationSnapshot`: dữ liệu snapshot để GUI đọc an toàn.
- `std::thread`.
- `std::mutex`.
- `std::lock_guard`.
- `std::atomic`.
- Cơ chế dừng thread bằng `requestStop()` và `join()`.

Module không tự định nghĩa lại:

- `Vehicle`.
- `TrafficLight`.
- Logic điều phối giao thông.
- Giao diện SFML.

## Cấu trúc thư mục

```text
SmartTrafficSimulation/
├── CMakeLists.txt
├── README.md
├── .gitignore    
└── src/
    ├── ConcurrencyModule.cpp
    └── concurrency/
        ├── CMakeLists1.txt
        ├── SimulationSnapshot.h
        ├── SnapshotStore.h
        ├── SnapshotStore.cpp
        ├── SimulationThread.h
        └── SimulationThread.cpp
