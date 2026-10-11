# Module contract — Người 4

## Mục tiêu

Người 4 chuẩn bị điểm tích hợp và kiểm thử, không sở hữu logic bên trong `Vehicle`, `TrafficLight`, traffic logic, SFML hay concurrency.

## Nguyên tắc

1. Không sửa trực tiếp branch của thành viên khác.
2. Không tự định nghĩa lại các kiểu dùng chung như `Direction`, `Movement`, `LightColor`.
3. Chỉ nối module qua API đã được chủ module xác nhận.
4. Không giả định tên hàm cụ thể trước khi API cuối cùng được chốt.
5. Giữ adapter ở một nơi để thay đổi API sau này chỉ cần sửa adapter, không sửa toàn bộ phần kiểm thử.

## Thứ tự tích hợp dự kiến

1. Core (`Vehicle` + `TrafficLight`) — do Người 1 cung cấp.
2. Traffic logic — do Người 2 cung cấp.
3. Tick/concurrency và snapshot — do Người 3 cung cấp.
4. GUI/renderer — do Người 5 cung cấp.

`IntegrationCoordinator` hiện là khung callback tổng quát, không gọi trực tiếp bất kỳ API nào của các module. Khi API được chốt, đăng ký callback thật tại điểm khởi tạo ứng dụng.

## Các quyết định cần nhóm xác nhận

- Tên hàm khởi tạo/cập nhật của core.
- API mà logic giao thông cần để đọc trạng thái đèn và cập nhật xe.
- Cách tạo `VehicleSnapshot` / `LightSnapshot` từ dữ liệu thật.
- Ai sở hữu vòng đời `SimulationThread` và gọi `requestStop()` / `join()`.
- Renderer nhận snapshot theo kiểu nào và tần suất đọc.
