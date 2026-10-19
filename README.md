# SmartTrafficSimulation — Concurrency module

Module này là phần của **Người 3 – Concurrency / Thread**. Module không sở hữu và không định nghĩa lại `Vehicle`, `TrafficLight` hay logic giao thông. Nó cung cấp:

- `SimulationThread`: quản lý luồng mô phỏng, dừng và `join()` an toàn.
- `SnapshotStore`: đồng bộ dữ liệu dùng chung giữa luồng mô phỏng và GUI.
- `SimulationSnapshot`: hợp đồng dữ liệu để GUI đọc bản sao mà không giữ khóa khi render.
- `VehicleSnapshot::rotationAngleDegrees`: góc xoay xe theo độ để GUI hiển thị đúng hướng; xe `UTurn` thường là `180.0`.
- `TurnCurve`: tiện ích Bézier bậc hai để Core tính vị trí và góc tiếp tuyến của xe khi quay đầu.

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

Mỗi `VehicleSnapshot` cần truyền thêm góc xoay hiển thị:

```cpp
vehicleSnapshots.push_back({
    vehicle.getId(),
    vehicle.getX(),
    vehicle.getY(),
    vehicle.getDirection(),
    vehicle.getMovement(),
    vehicle.getRotationAngleDegrees()
});
```

### Người 5 — GUI / SFML

Trong vòng lặp ở luồng chính:

```cpp
const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
renderer.render(snapshot);
```

`getSnapshot()` trả về bản sao. GUI không được giữ mutex trong lúc vẽ.

GUI dùng góc xoay như sau:

```cpp
for (const VehicleSnapshot& vehicle : snapshot.vehicles) {
    sprite.setPosition(
        static_cast<float>(vehicle.x),
        static_cast<float>(vehicle.y)
    );
    sprite.setRotation(
        static_cast<float>(vehicle.rotationAngleDegrees)
    );
    window.draw(sprite);
}
```

Quy ước góc theo độ:

- `0.0`: không xoay.
- `90.0`: xoay một góc vuông.
- `180.0`: quay đầu (`Movement::UTurn`).

Nếu hướng gốc của sprite khác hướng quy ước của simulation, GUI có thể cộng hoặc trừ một offset hiển thị riêng.

Core có thể tính xe quay đầu theo đường cong Bézier bậc hai:

```cpp
const TurnCurveSample sample = TurnCurve::sampleQuadraticBezier(
    {100.0, 300.0},  // điểm bắt đầu
    {240.0, 190.0},  // điểm điều khiển, quyết định độ cong
    {100.0, 100.0},  // điểm kết thúc
    turnProgress     // từ 0.0 đến 1.0
);

vehicleSnapshot.x = sample.position.x;
vehicleSnapshot.y = sample.position.y;
vehicleSnapshot.rotationAngleDegrees = sample.rotationAngleDegrees;
```

Góc xoay được lấy từ vector tiếp tuyến của đường cong nên xe vừa di chuyển theo
đường cong vừa xoay tự nhiên theo hướng đang chạy. `TurnCurve` chỉ là tiện ích
tính toán; Core/Vehicle vẫn là nơi sở hữu trạng thái và quyết định `turnProgress`.

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


## Báo cáo sửa lỗi T2–T6

- **T2:** Không dùng `CMakeLists.txt` ở thư mục gốc của module này; chỉ giữ `src/concurrency/CMakeLists.txt`. Project chính của nhóm gọi module bằng `add_subdirectory(src/concurrency)` và tự khai báo `find_package(Threads REQUIRED)`.
- **T3–T4:** `examples/ConcurrencyPrototype.cpp` dùng class `SimulationEngine` có `update()`, `start()`, `stop()` và `getSnapshotStore()`. `SimulationThread` gọi `SimulationEngine::update()` trong worker thread; dữ liệu được publish qua `SnapshotStore`.
- **T5:** `SimulationThread::start()` khóa vòng đời, từ chối gọi lần hai khi đang chạy bằng `std::logic_error`, tự `join()` worker cũ trước khi restart sau khi worker đã dừng, và `requestStop()` đánh thức thread ngay bằng `condition_variable`. Destructor luôn `requestStop()` rồi `join()`.
- **T6:** Chạy build cảnh báo nghiêm ngặt và ThreadSanitizer trên prototype; không phát hiện race condition.

### CMake của project chính

CMake ở project chính cần có:

```cmake
find_package(Threads REQUIRED)
add_subdirectory(src/concurrency)
target_link_libraries(SmartTrafficSimulation PRIVATE concurrency)
```

Không build `examples/ConcurrencyPrototype.cpp` cùng executable chính nếu executable chính đã có `main.cpp` riêng.
