# SmartTrafficSimulation

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

> Cập nhật phần này khi nhóm chốt công cụ build (Visual Studio, CMake, g++...) và cách cài SFML.

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
git clone https://github.com/KhanhVys/SmartTrafficSimulation-group3-.git
cd SmartTrafficSimulation-group3-
git checkout <branch của bạn>

# sau khi sửa code
git add .
git commit -m "Nội dung thay đổi"
git push
```
