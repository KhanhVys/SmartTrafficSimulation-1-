# Giao tiếp giữa các module

> Do Người 4 soạn. Cả nhóm xem và góp ý. Người 3 đọc lại quy tắc khóa ở mục 4.
> Tên class, hàm theo quy ước nhóm. Muốn đổi phải báo nhóm trưởng.

## 1. Ai gọi ai

```
main
 ├─ Luồng mô phỏng ──► SimulationEngine::run()
 │      mỗi tick:
 │        1. SimulationClock::tick()
 │        2. TrafficLight::update(time)        (cho từng đèn)
 │        3. Vehicle::update(dt, WorldView)    (cho từng xe)
 │
 └─ Luồng chính (SFML) ──► Renderer::render(snapshot)
          lấy snapshot từ SimulationEngine::getSnapshot()
```

| Bên gọi | Gọi cái gì | Mục đích |
|---|---|---|
| `SimulationEngine` | `TrafficLight::update(time)` | Cập nhật pha đèn theo thời gian |
| `SimulationEngine` | `Vehicle::update(dt, view)` | Cho xe di chuyển, đổi state |
| `Vehicle` (qua state) | `TrafficLight::getColor(...)` | Hỏi đèn của hướng mình (chỉ đọc) |
| `Vehicle` (qua state) | `Road` / `Intersection` | Hỏi vị trí vạch dừng, xe phía trước (chỉ đọc) |
| `TrafficLight` | `EventBus::publish(LightChanged)` | Báo đèn đổi pha |
| `Renderer` | `SimulationEngine::getSnapshot()` | Lấy dữ liệu để vẽ (chỉ đọc) |

Quy tắc: **Renderer không bao giờ sửa dữ liệu mô phỏng.** Xe không sửa đèn, đèn không sửa xe.

## 2. Dữ liệu dùng chung, ai đọc, ai ghi

| Dữ liệu | Ai ghi (duy nhất) | Ai đọc | Khóa |
|---|---|---|---|
| Danh sách xe (vị trí, làn, state) | `SimulationEngine` (luồng mô phỏng) | `Renderer`, `Vehicle` khác | `vehicleMutex` |
| Trạng thái đèn (màu từng hướng) | `TrafficLight` (luồng mô phỏng) | `Vehicle`, `Renderer` | `lightMutex` |
| Thời gian mô phỏng | `SimulationClock` | Mọi module | atomic |
| Cấu trúc đường (`RoadNetwork`) | Chỉ ghi lúc khởi tạo | Mọi module | Không cần (bất biến sau khi tạo) |

Mỗi dữ liệu chỉ có **một nơi ghi**, nhờ vậy giảm race condition.

## 3. Các kiểu dùng chung (để mọi người dùng cùng một tên)

```cpp
enum class Direction { North, South, East, West };      // hướng đường vào ngã tư
enum class Movement  { Straight, Left, Right, UTurn };  // hướng xe định đi
enum class LightColor { Red, Yellow, Green };
```

- `LightColor` chỉ là giá trị trả về để vẽ và hỏi đèn. Logic đèn vẫn nằm trong `RedState`, `YellowState`, `GreenState` (đúng quy ước: không thay State class bằng enum).
- Đèn có 4 nhóm: **BN-Thẳng, BN-Trái/Quay, ĐT-Thẳng, ĐT-Trái/Quay**. Rẽ phải đi chung đèn với đi thẳng.
- Thời gian mô phỏng tính bằng **tick**: **1 tick = 0.02 giây** (50 tick = 1 giây). Thời gian đèn nên lưu bằng **số tick (kiểu số nguyên)** để không bị lệch do số thực.
- Thời lượng đèn **theo biên bản họp** (xem mục 7). Nên khai báo thành hằng số để sau này đổi số mà không phải sửa logic.

## 4. Đa luồng (đã chốt 2 luồng)

Hai luồng:

| Luồng | Việc |
|---|---|
| Luồng mô phỏng | Chạy `SimulationEngine::run()`: cập nhật đèn, cập nhật xe |
| Luồng chính | Vòng lặp SFML: xử lý sự kiện, vẽ |

Lý do tách: SFML nên giữ cửa sổ ở luồng chính. Người 3 vẫn nên đọc lại quy tắc khóa bên dưới. Nếu muốn tách thêm luồng thì cần thống nhất lại.

Quy tắc khóa:
1. Dùng `std::lock_guard`, giữ khóa **càng ngắn càng tốt**.
2. `getSnapshot()` chỉ **sao chép** dữ liệu trong lúc giữ khóa, rồi trả về bản sao. `Renderer` vẽ trên bản sao, không giữ khóa lúc vẽ.
3. Nếu cần khóa cả hai: luôn khóa **`lightMutex` trước, `vehicleMutex` sau** (tránh deadlock).

## 5. Quy ước vận hành đã chốt cho code

- Xe sinh một lần lúc khởi động: **13 xe**, không sinh thêm.
- Xe không chuyển làn, đi theo làn cố định.
- **Đường đôi = hướng Đông-Tây (ĐT)**, 3 làn mỗi chiều: làn trái = rẽ trái + quay đầu, làn giữa = đi thẳng, làn phải = rẽ phải.
- **Đường đơn = hướng Bắc-Nam (BN)**, 1 làn mỗi chiều: làn đủ rộng cho 2 xe đứng song song, chia 2 hàng: một hàng rẽ trái + quay đầu, một hàng đi thẳng + rẽ phải. Xe rẽ trái đang chờ không chặn xe đi thẳng.
- `EventBus` để sau, chưa dùng ở bản MVP. Đèn và xe giao tiếp qua `getColor()`.
- Xe chưa qua vạch dừng khi đèn đổi pha: giảm tốc và dừng. Đã qua vạch dừng: đi tiếp.

## 6. Việc chưa chốt

Hiện không còn mục nào. Phát sinh thì ghi thêm vào đây.

## 7. Thời lượng đèn (theo biên bản họp)

Thứ tự mỗi hướng: **Xanh, Vàng, Đỏ**. Giữa các pha có **đỏ toàn bộ** để xe dọn sạch ngã tư.

| Pha | Xanh | Vàng | Đỏ toàn bộ | Giây trong chu kỳ |
|---|---|---|---|---|
| BN thẳng + rẽ phải | 6 giây | 2 | 2 | 0 đến 9 |
| BN rẽ trái + quay đầu | 5 giây | 2 | 2 | 10 đến 18 |
| ĐT thẳng + rẽ phải | 6 giây | 2 | 2 | 19 đến 28 |
| ĐT rẽ trái + quay đầu | 5 giây | 2 | 2 | 29 đến 37 |
| **Cả chu kỳ** | | | | **38 giây = 1900 tick** |

Hết giây 37 thì quay lại giây 0. Với 1 tick = 0.02 giây, mỗi giây là 50 tick.
Nên lưu thời lượng bằng số tick (số nguyên), ví dụ `STRAIGHT_GREEN_TICKS = 300` (6 giây).
