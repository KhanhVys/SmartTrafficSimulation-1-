# SmartTrafficSimulation — Concurrency module

Module này là phần của **Người 3 – Concurrency / Thread**. Module không sở hữu và không định nghĩa lại `Vehicle`, `TrafficLight` hay logic giao thông. Nó cung cấp:

- `SimulationThread`: quản lý luồng mô phỏng, dừng và `join()` an toàn.
- `SnapshotStore`: đồng bộ dữ liệu dùng chung giữa luồng mô phỏng và GUI.
- `SimulationSnapshot`: hợp đồng dữ liệu để GUI đọc bản sao mà không giữ khóa khi render.

## File cần merge vào project chính

Trưởng nhóm nên merge các file module hóa sau:

```text
src/concurrency/SimulationSnapshot.h
src/concurrency/SnapshotStore.h
src/concurrency/SnapshotStore.cpp
src/concurrency/SimulationThread.h
src/concurrency/SimulationThread.cpp
```

Các file sau là cấu hình và tài liệu nên commit cùng branch:

```text
CMakeLists.txt
README.md
.gitignore
```

`src/ConcurrencyModule.cpp` và `examples/ConcurrencyPrototype.cpp` là prototype có
`main()` riêng. Có thể giữ trên Git để demo, nhưng không được thêm cả hai vào
executable mô phỏng chính.

## Build và chạy prototype

```bash
cmake -S . -B build
cmake --build build
./build/concurrency_prototype
```

Để build thêm prototype một-file:

```bash
cmake -S . -B build-single -DBUILD_STANDALONE_SINGLE_FILE=ON
cmake --build build-single
./build-single/concurrency_single_file
```

Hoặc build nhanh:

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic \
  src/concurrency/SimulationThread.cpp \
  src/concurrency/SnapshotStore.cpp \
  examples/ConcurrencyPrototype.cpp \
  -Isrc -o concurrency_prototype
```

## Hợp đồng tích hợp với các thành viên

### Người 1 — Core Simulation

Trong callback mỗi tick, Core cập nhật `TrafficLight` và `Vehicle`, sau đó xuất dữ liệu phẳng:

```cpp
snapshotStore.publishTick(currentTick);
snapshotStore.publishLights(lightSnapshots);
snapshotStore.publishVehicles(vehicleSnapshots);
```

`lightSnapshots` và `vehicleSnapshots` được tạo từ dữ liệu của Core; module Concurrency không truy cập trực tiếp vào state nội bộ của Core.

### Người 5 — GUI / SFML

Trong vòng lặp ở luồng chính:

```cpp
const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
renderer.render(snapshot);
```

`getSnapshot()` trả về bản sao. GUI không được giữ mutex trong lúc vẽ.

### Người 4 — Tích hợp / Git

Có thể tích hợp module bằng subtree hoặc merge branch:

```bash
git checkout -b feature/concurrency
git add src/concurrency examples CMakeLists.txt README.md .gitignore
git commit -m "feat: add thread-safe simulation concurrency module"
git push -u origin feature/concurrency
```

Sau đó mở Pull Request để Người 4 review. Nếu project chính đã có `CMakeLists.txt`, chỉ cần giữ lại target `concurrency`, include directory `src` và link thư viện này vào executable chính.

## Cách ghép vào executable chính

Sau khi merge branch, `CMakeLists.txt` của project chính cần link thư viện:

```cmake
add_subdirectory(src/concurrency)

target_link_libraries(SmartTrafficSimulation
    PRIVATE concurrency
)
```

`main.cpp` của nhóm tạo `SimulationEngine`, khởi động `SimulationThread`,
để callback gọi `SimulationEngine::update()`, rồi cho SFML đọc:

```cpp
const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
renderer.render(window, snapshot);
```

Khi tích hợp thật, chỉ giữ một `main()` của project chính. Không link
`concurrency_prototype` hoặc `concurrency_single_file` vào executable đó.

## Quy tắc khóa

- `vehicleMutex` chỉ bảo vệ danh sách xe.
- `lightMutex` chỉ bảo vệ trạng thái đèn.
- `getSnapshot()` khóa theo thứ tự **`lightMutex` trước, `vehicleMutex` sau**.
- Chỉ sao chép dữ liệu trong vùng khóa; không gọi GUI/Core callback khi đang giữ khóa.
- Chỉ một nơi ghi dữ liệu mô phỏng; GUI chỉ đọc snapshot.
