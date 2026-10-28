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
=======
Phần mềm **mô phỏng giao thông đô thị thời gian thực** viết bằng C++ cho môn Lập trình Hướng đối tượng (OOP).

Xe, đèn giao thông và giao lộ được mô hình hóa thành các đối tượng tự vận hành, có khả năng tự điều chỉnh hành vi.

## Kiến trúc và kỹ thuật chính

- **State Pattern**: trạng thái đèn giao thông và hành vi của xe
- **Composite Pattern**: cấu trúc mạng lưới đường
- **Multithreading**: xử lý đồng thời thật bằng `std::thread`, `mutex`, `lock_guard`
- **SFML**: hiển thị đồ họa

## Các class chính

| Phần | Class |
|---|---|
| Xe | `Vehicle` |
| Đèn giao thông | `TrafficLight` |
| Trạng thái xe / đèn | `IVehicleState` / `ILightState` |
| Đường / Ngã tư | `Road` / `Intersection` |
| Thành phần đường (Composite) | `IRoadComponent` |
| Mạng đường | `RoadNetwork` |
| Đồng hồ mô phỏng | `SimulationClock` |
| Hệ thống sự kiện | `EventBus` |
| Bộ điều khiển mô phỏng | `SimulationEngine` |
| Sinh xe | `TrafficGenerator` |
| Hiển thị | `Renderer` |

**State của xe:** `AcceleratingState`, `BrakingState`, `WaitingState`, `TurningState`, `CruisingState`
**State của đèn:** `RedState`, `YellowState`, `GreenState`

## Mô hình đã chốt (theo biên bản họp)

| Hạng mục | Nội dung |
|---|---|
| Loại mô hình | Ngã tư cắt ngang bởi đường đôi |
| Số ngã tư | 1 |
| Đường đơn (hướng Bắc-Nam) | 2 chiều, mỗi chiều 1 làn (tổng 2 làn). Mỗi làn đủ rộng cho 2 xe đứng song song: một hàng cho rẽ trái + quay đầu, một hàng cho đi thẳng + rẽ phải |
| Đường đôi (hướng Đông-Tây) | Có dải phân cách, mỗi chiều 3 làn (tổng 6 làn). Làn trái: rẽ trái + quay đầu, làn giữa: đi thẳng, làn phải: rẽ phải |
| Hướng xe | Đi thẳng, rẽ trái, rẽ phải, quay đầu |
| Chuyển làn | Không cho phép, xe đi theo làn cố định |
| Loại xe | Chỉ ô tô |
| Số xe | 13 xe ban đầu, cố định, không sinh thêm |
| Tốc độ xe | Tùy chỉnh được |
| Cách rẽ | Rẽ theo đường cong |
| Ưu tiên lưu thông | Theo tín hiệu đèn |
| Bước mô phỏng | 1 tick = 0.02 giây (50 tick = 1 giây) |
| EventBus | Để sau, chưa dùng ở bản MVP |
| Luồng chạy | Luồng mô phỏng (cập nhật xe + đèn) và luồng chính (SFML: cửa sổ + vẽ) |
| Hiển thị | SFML, có xi-nhan cho rẽ trái và quay đầu |
| Phạm vi bản đầu (MVP) | Mô hình chạy được với các mục trên |

### Quy tắc xe theo đèn

- Xe chưa qua vạch dừng khi đèn chuyển pha: giảm tốc và dừng lại.
- Xe đã qua vạch dừng: tiếp tục đi, không giảm tốc.
- Xe chờ đèn đỏ xếp hàng tại vạch dừng theo làn.

### Chu kỳ đèn (38 giây)

Thứ tự mỗi hướng: **Xanh, Vàng, Đỏ**. Hai nhóm hướng đi chung pha: **thẳng + rẽ phải** và **rẽ trái + quay đầu**.
Ký hiệu: **BN** = Bắc-Nam, **ĐT** = Đông-Tây.
Thời lượng đèn theo biên bản họp. Nên khai báo thành hằng số để sau này đổi số mà không sửa logic.

| Giây | Pha đang chạy |
|---|---|
| 0-5 | BN thẳng + rẽ phải: Xanh |
| 6-7 | BN thẳng + rẽ phải: Vàng |
| 8-9 | Đỏ toàn bộ |
| 10-14 | BN rẽ trái + quay đầu: Xanh |
| 15-16 | BN rẽ trái + quay đầu: Vàng |
| 17-18 | Đỏ toàn bộ |
| 19-24 | ĐT thẳng + rẽ phải: Xanh |
| 25-26 | ĐT thẳng + rẽ phải: Vàng |
| 27-28 | Đỏ toàn bộ |
| 29-33 | ĐT rẽ trái + quay đầu: Xanh |
| 34-35 | ĐT rẽ trái + quay đầu: Vàng |
| 36-37 | Đỏ toàn bộ |
| 38 | Quay lại giây 0 |

### Chưa chốt

Hiện không còn mục nào.

## Phân công

| Thành viên | Phần việc | Branch |
|---|---|---|
| Người 1 | Core Simulation (Vehicle, TrafficLight), slide thuyết trình | `feature/vehicle` |
| Người 2 | Logic / Thuật toán điều phối giao thông | `feature/logic` |
| Người 3 | Concurrency / Thread | `feature/concurrency` |
| Người 4 | Tích hợp, kiểm thử, review code | `feature/integration` |
| Người 5 | GUI (SFML) | `feature/gui` |

> Ghi tên thành viên vào bảng này khi nhóm chốt.

## Mốc thời gian

| Mốc | Nội dung |
|---|---|
| 21/9 | Khởi động, thống nhất cấu trúc và quy ước |
| 28/9 | Cấu trúc project và prototype thread chạy độc lập |
| 12/10 | Core Simulation ổn định, bàn giao tích hợp |
| 19/10 | Tích hợp lần 1, GUI kết nối dữ liệu thật |
| 25/10 | Test toàn hệ thống, sửa lỗi |
| 30/10 | Freeze bản chính thức |
| 31/10 | Demo và thuyết trình |

## Cấu trúc thư mục

```
SmartTrafficSimulation/
├── src/        # mã nguồn (.h và .cpp)
├── docs/       # báo cáo, slide
├── .gitignore
└── README.md
```

## Cách build

**Yêu cầu:** C++17 (GCC 13 trở lên hoặc MSVC 2022), CMake 3.16 trở lên, Ninja (Windows: `winget install Ninja-build.Ninja`).

```bash
cmake -S . -B build -G Ninja -DBUILD_GUI=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

- `-DBUILD_GUI=OFF`: bỏ qua phần đồ họa. Khi cần GUI, bỏ tham số này, CMake sẽ tự tải **SFML 3** (lần đầu cần có mạng).
- Dự án dùng chung kiểu dữ liệu trong `src/common/Types.h`. Không tự định nghĩa lại `Direction`, `Movement`, `LightColor`.
- Không commit thư mục `build/` và file `.exe`.

## Quy ước đặt tên

| Thành phần | Quy tắc | Ví dụ |
|---|---|---|
| File | PascalCase, trùng tên class | `Vehicle.h`, `Vehicle.cpp` |
| Class | PascalCase | `TrafficLight` |
| State class | PascalCase + hậu tố `State` | `RedState` |
| Hàm | camelCase | `update()`, `addComponent()` |
| Biến | camelCase | `vehicleId`, `currentTick` |
| Hằng số | CHỮ_HOA_GẠCH_DƯỚI | `MAX_VEHICLES` |
| Enum | PascalCase | `enum class Direction { North, South, East, West };` |

- File `.h` để khai báo, `.cpp` để cài đặt.
- Dùng `#pragma once` cho mọi header.
- Chỉ `#include` thư viện cần thiết.
- Comment để giải thích logic khó hiểu, không comment những dòng quá hiển nhiên.

## Quy tắc Git

1. Mỗi người làm trên **branch riêng**, chỉ push vào branch của mình.
2. **Không push thẳng lên `main`.** Muốn đưa code vào `main`, tạo **Pull Request** và chờ Người 4 duyệt.
3. **Không push code chưa compile được.**
4. Commit ghi rõ nội dung thay đổi (ví dụ `Add TrafficLight state classes`).
5. Không tự ý sửa hoặc xóa code của người khác khi chưa trao đổi.
6. Không tự ý đổi tên class, file, interface đã thống nhất. Muốn đổi phải báo nhóm trưởng.
7. Trước khi merge phải kiểm tra code và build lại project.

### Lấy code về và làm việc

```bash
git clone https://github.com/KhanhVys/SmartTrafficSimulation-1-.git
cd SmartTrafficSimulation-1-
git checkout <branch của bạn>

# sau khi sửa code
git add .
git commit -m "Nội dung thay đổi"
git push
```

