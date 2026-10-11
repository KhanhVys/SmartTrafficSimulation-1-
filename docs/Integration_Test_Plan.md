# Integration test plan — Người 4

Các test dưới đây là kịch bản cần chạy khi module tương ứng được bàn giao. Chưa đánh dấu PASS trước khi chạy trên code tích hợp thật.

| ID | Phạm vi | Tình huống | Kết quả mong đợi | Trạng thái ban đầu |
|---|---|---|---|---|
| IT-01 | Build | Build project từ sạch | Build thành công, không có định nghĩa trùng | BLOCKED — chờ module hoàn chỉnh |
| IT-02 | Core | Khởi tạo số xe ban đầu | Đúng số xe đã thống nhất | BLOCKED |
| IT-03 | Đèn + logic | Đèn đỏ cho hướng xe | Xe chưa qua vạch dừng lại | BLOCKED |
| IT-04 | Đèn + logic | Xe đã qua vạch thì đèn đổi đỏ | Xe tiếp tục đi qua nút giao | BLOCKED |
| IT-05 | Pha đèn | Chuyển xanh → vàng → đỏ/pha kế tiếp | Đúng thứ tự và thời lượng đã chốt | BLOCKED |
| IT-06 | Concurrency | Chạy mô phỏng và đọc snapshot đồng thời | Không crash, snapshot không bị đọc/ghi cạnh tranh | BLOCKED |
| IT-07 | Snapshot | Đọc snapshot rồi render | Render không giữ mutex của mô phỏng | BLOCKED |
| IT-08 | Shutdown | Đóng cửa sổ khi thread đang chạy | Thread được yêu cầu dừng và join an toàn | BLOCKED |
| IT-09 | GUI | Xe/đèn trên GUI dùng dữ liệu thật | Vị trí và màu đèn phản ánh snapshot hiện tại | BLOCKED |
| IT-10 | Full scenario | Chạy ít nhất một kịch bản hoàn chỉnh | Xe tuân thủ pha đèn, GUI và simulation nhất quán | BLOCKED |

## Mẫu ghi bug

- Bug ID:
- Branch/commit:
- Bước tái hiện:
- Kết quả thực tế:
- Kết quả mong đợi:
- Module phụ trách:
- Mức độ: Blocker / High / Medium / Low
- Trạng thái: Open / In progress / Fixed / Retest
