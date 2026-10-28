# SmartTrafficSimulation

## Module Concurrency / Thread

Đây là module **Người 3 – Concurrency / Thread** của project mô phỏng giao thông thông minh. Module chịu trách nhiệm quản lý thread mô phỏng và đồng bộ dữ liệu giữa Simulation Core với các thành phần đọc snapshot.

Module này không sở hữu logic chính của `Vehicle`, `TrafficLight` hoặc luật giao thông. Core/Logic vẫn là nơi cập nhật trạng thái thật; module Concurrency chỉ quản lý luồng, lưu trữ và cung cấp bản sao dữ liệu an toàn.

## Chức năng chính

Module cung cấp:

- `SimulationThread`: chạy callback mô phỏng trên worker thread.
- `SnapshotStore`: lưu và đồng bộ snapshot bằng `mutex` và `lock_guard`.
- `SimulationSnapshot`: hợp đồng dữ liệu giữa Simulation Core và thành phần hiển thị.
- `Types.h`: nơi định nghĩa tập trung các enum dùng chung.
- `TurnCurve.h`: tính quỹ đạo quay đầu bằng đường cong Bézier bậc hai.
- Cơ chế dừng và `join()` thread an toàn.
- Bảo vệ trường hợp gọi `start()` nhiều lần.
- Kiểm tra race condition bằng ThreadSanitizer.

## Cấu trúc thư mục

```text
SmartTrafficSimulation/
├── .gitignore
├── README.md
├── CONCURRENCY.md
├── examples/
│   └── ConcurrencyPrototype.cpp
└── src/
    ├── ConcurrencyModule.cpp
    ├── common/
    │   ├── Types.h
    │   └── TurnCurve.h
    └── concurrency/
        ├── CMakeLists.txt
        ├── SimulationSnapshot.h
        ├── SnapshotStore.h
        ├── SnapshotStore.cpp
        ├── SimulationThread.h
        └── SimulationThread.cpp
```

`README.md` là tài liệu tổng quan của project. `CONCURRENCY.md` là tài liệu chi tiết riêng cho module Concurrency nếu nhóm giữ file này.

## Quy ước kiểu dữ liệu dùng chung

Các enum dùng chung chỉ được định nghĩa một lần trong:

```text
src/common/Types.h
```

Nội dung hiện tại:

```cpp
#pragma once

enum class LightColor { Red, Yellow, Green };
enum class Direction { North, South, East, West };
enum class Movement { Straight, Left, Right, UTurn };
```

Các file khác phải include:

```cpp
#include "common/Types.h"
```

Không định nghĩa lại `LightColor`, `Direction` hoặc `Movement` trong các module khác, vì sẽ gây lỗi định nghĩa trùng khi tích hợp.

## Snapshot data contract

### `VehicleSnapshot`

```cpp
struct VehicleSnapshot {
    int vehicleId = 0;
    double x = 0.0;
    double y = 0.0;
    Direction direction = Direction::North;
    Movement movement = Movement::Straight;
    bool signalingLeft = false;
    double rotationAngleDegrees = 0.0;
};
```

Ý nghĩa các trường:

- `vehicleId`: mã xe.
- `x`, `y`: vị trí xe.
- `direction`: hướng di chuyển chính.
- `movement`: đi thẳng, rẽ trái, rẽ phải hoặc quay đầu.
- `signalingLeft`: xe có đang bật xi-nhan trái hay không.
- `rotationAngleDegrees`: góc xoay của sprite/xe khi hiển thị.

Khi tạo snapshot từ `Vehicle` thật:

```cpp
VehicleSnapshot snapshot{
    vehicle.getId(),
    vehicle.getX(),
    vehicle.getY(),
    vehicle.getDirection(),
    vehicle.getMovement(),
    vehicle.isSignalingLeft(),
    vehicle.getRotationAngleDegrees()
};
```

### `LightSnapshot`

```cpp
struct LightSnapshot {
    Direction direction = Direction::North;
    bool turnArrow = false;
    LightColor color = LightColor::Red;
};
```

`turnArrow` phải được đặt trước `color` khi dùng aggregate initializer:

```cpp
LightSnapshot lightSnapshot{
    Direction::North,
    true,
    LightColor::Green
};
```

`turnArrow` biểu thị mũi tên rẽ đang được phép theo pha đèn. Logic quyết định giá trị này thuộc về Traffic Light/Core.

### `SimulationSnapshot`

```cpp
struct SimulationSnapshot {
    std::size_t tick = 0;
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
```

`SnapshotStore::getSnapshot()` trả về bản sao dữ liệu. Thành phần đọc snapshot không giữ mutex trong lúc xử lý hoặc render.

## `SimulationThread`

API chính:

```cpp
class SimulationThread {
public:
    using TickFunction = std::function<void(std::size_t)>;

    SimulationThread(
        TickFunction tickFunction,
        std::chrono::milliseconds tickInterval
    );

    ~SimulationThread();

    void start();
    void requestStop();
    void join();
    bool isRunning() const;
};
```

### Khởi động và dừng

```cpp
SimulationThread simulationThread(
    [&engine](std::size_t tick) {
        engine.update(tick);
    },
    std::chrono::milliseconds(20)
);

simulationThread.start();

// Khi đóng ứng dụng:
simulationThread.requestStop();
simulationThread.join();
```

Destructor cũng gọi `requestStop()` và `join()` để tránh worker thread còn chạy khi object bị hủy.

### Gọi `start()` lần hai

Nếu thread đang chạy, gọi `start()` lần hai sẽ bị từ chối bằng exception:

```cpp
try {
    simulationThread.start();
} catch (const std::logic_error& error) {
    std::cerr << error.what() << '\n';
}
```

Cách này tránh việc tạo thread trùng hoặc làm chương trình gọi `std::terminate()`.

Sau khi đã dừng và `join()`, thread có thể được khởi động lại an toàn:

```cpp
simulationThread.requestStop();
simulationThread.join();

simulationThread.start();
```

## `SnapshotStore`

API sử dụng:

```cpp
snapshotStore.publishTick(currentTick);
snapshotStore.publishVehicles(vehicleSnapshots);
snapshotStore.publishLights(lightSnapshots);
```

Lấy snapshot:

```cpp
const SimulationSnapshot snapshot =
snapshotStore.getSnapshot();
```

`SnapshotStore` sử dụng:

- `vehicleMutex` để bảo vệ danh sách xe.
- `lightMutex` để bảo vệ danh sách đèn.
- `lock_guard` để khóa tự động và mở khóa khi ra khỏi scope.
- `atomic` để lưu tick hiện tại.

Khi lấy cả xe và đèn, thứ tự khóa cố định là:

```text
lightMutex -> vehicleMutex
```

Mọi code mở rộng `SnapshotStore` phải giữ cùng thứ tự để tránh deadlock.

## Luồng cập nhật dữ liệu

Luồng dữ liệu dự kiến:

```text
SimulationThread
        |
        v
SimulationEngine::update()
        |
        +--> Vehicle / TrafficLight thật
        |
        +--> publishVehicles()
        +--> publishLights()
        +--> publishTick()
        |
        v
SnapshotStore
        |
        v
getSnapshot()
```

Ví dụ trong Simulation Engine:

```cpp
void SimulationEngine::update(std::size_t currentTick) {
    updateVehicles();
    updateTrafficLights();

    std::vector<VehicleSnapshot> vehicleSnapshots;
    std::vector<LightSnapshot> lightSnapshots;

    for (const Vehicle& vehicle : vehicles) {
        vehicleSnapshots.push_back({
            vehicle.getId(),
            vehicle.getX(),
            vehicle.getY(),
            vehicle.getDirection(),
            vehicle.getMovement(),
            vehicle.isSignalingLeft(),
            vehicle.getRotationAngleDegrees()
        });
    }

    for (const TrafficLight& light : trafficLights) {
        lightSnapshots.push_back({
            light.getDirection(),
            light.hasTurnArrow(),
            light.getColor()
        });
    }

    snapshotStore.publishTick(currentTick);
    snapshotStore.publishVehicles(vehicleSnapshots);
    snapshotStore.publishLights(lightSnapshots);
}
```

## Quỹ đạo xe quay đầu

File:

```text
src/common/TurnCurve.h
```

`TurnCurve` sử dụng đường cong Bézier bậc hai:

```cpp
const TurnCurveSample sample =
    TurnCurve::sampleQuadraticBezier(
        startPoint,
        controlPoint,
        endPoint,
        turnProgress
    );
```

Trong đó `turnProgress` nằm trong khoảng `0.0` đến `1.0`.

- `0.0`: bắt đầu quay.
- `1.0`: kết thúc quay.
- `controlPoint`: quyết định độ cong của quỹ đạo.

Kết quả trả về:

```cpp
struct TurnCurveSample {
    Point2D position;
    double rotationAngleDegrees = 0.0;
};
```

Cập nhật vào snapshot:

```cpp
vehicleSnapshot.x = sample.position.x;
vehicleSnapshot.y = sample.position.y;
vehicleSnapshot.rotationAngleDegrees =
    sample.rotationAngleDegrees;
```

Góc xoay được tính từ vector tiếp tuyến của đường cong, vì vậy xe thay đổi hướng tự nhiên trong lúc di chuyển.

## Build trực tiếp bằng g++

Do project module hiện không giữ `CMakeLists.txt` ở thư mục gốc, có thể build prototype trực tiếp bằng g++.

### Prototype module hóa

Linux/macOS/WSL:

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic \
  src/concurrency/SimulationThread.cpp \
  src/concurrency/SnapshotStore.cpp \
  examples/ConcurrencyPrototype.cpp \
  -Isrc \
  -o concurrency_prototype
```

Chạy:

```bash
./concurrency_prototype
```

Windows PowerShell dùng MinGW:

```powershell
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic `
  src/concurrency/SimulationThread.cpp `
  src/concurrency/SnapshotStore.cpp `
  examples/ConcurrencyPrototype.cpp `
  -Isrc `
  -o concurrency_prototype.exe
```

Chạy:

```powershell
.\concurrency_prototype.exe
```

### Prototype một file

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic \
  src/ConcurrencyModule.cpp \
  -Isrc \
  -o concurrency_single_file
```

## ThreadSanitizer

Build bản kiểm tra race condition:

```bash
g++ -fsanitize=thread \
  -fno-omit-frame-pointer \
  -g \
  -std=c++17 \
  -pthread \
  src/concurrency/SimulationThread.cpp \
  src/concurrency/SnapshotStore.cpp \
  examples/ConcurrencyPrototype.cpp \
  -Isrc \
  -o concurrency_tsan
```

Chạy:

```bash
./concurrency_tsan
```

Kết quả đạt yêu cầu là chương trình không xuất hiện:

```text
WARNING: ThreadSanitizer: data race
```

## CMake khi tích hợp vào project chính

File module:

```text
src/concurrency/CMakeLists.txt
```

Project chính của nhóm cần tự khai báo:

```cmake
find_package(Threads REQUIRED)
add_subdirectory(src/concurrency)

target_link_libraries(SmartTrafficSimulation
    PRIVATE concurrency
)
```

Không thêm `examples/ConcurrencyPrototype.cpp` hoặc `src/ConcurrencyModule.cpp` vào executable chính nếu executable chính đã có `main.cpp` riêng.

## Quy tắc tích hợp Git

Branch làm việc của module:

```text
feature/concurrency
```

Cập nhật branch:

```bash
git checkout feature/concurrency
git fetch origin
git pull --rebase origin feature/concurrency
```

Commit thay đổi:

```bash
git add .
git commit -m "feat: update concurrency snapshot contract"
```

Push branch:

```bash
git push origin feature/concurrency
```

Sau đó tạo Pull Request:

```text
base: main
compare: feature/concurrency
```

Nếu Pull Request báo conflict, cập nhật `main` vào branch cá nhân:

```bash
git checkout feature/concurrency
git fetch origin
git merge origin/main
```

Sửa các file conflict, sau đó:

```bash
git add .
git commit -m "merge: resolve conflicts with main"
git push origin feature/concurrency
```

Không sử dụng `git push --force` lên `main`.

## Checklist bàn giao

- [ ] `Types.h` là nơi duy nhất định nghĩa các enum dùng chung.
- [ ] `SimulationSnapshot.h` include `common/Types.h`.
- [ ] `VehicleSnapshot` có `signalingLeft`.
- [ ] `LightSnapshot` có `turnArrow` trước `color`.
- [ ] `SimulationThread` có `start()`, `requestStop()` và `join()` an toàn.
- [ ] Gọi `start()` lần hai được xử lý bằng exception, không crash.
- [ ] Snapshot được publish bằng `SnapshotStore`.
- [ ] Không truy cập trực tiếp vector dùng chung từ thread khác.
- [ ] Không còn `CMakeLists.txt` ở thư mục gốc nếu project chính đã quản lý CMake.
- [ ] Chạy build với `-Wall -Wextra -Wpedantic` thành công.
- [ ] Chạy ThreadSanitizer không phát hiện data race.
- [ ] Commit đã được push lên `feature/concurrency`.
