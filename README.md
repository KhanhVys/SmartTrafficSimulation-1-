# SmartTrafficSimulation-group3-Logic

## 1. Giới thiệu

Module Logic chịu trách nhiệm điều phối giao thông tại giao lộ, quản lý chu kỳ đèn tín hiệu và xác định quy tắc di chuyển của phương tiện.

## 2. Các file chính

* `TrafficController.h/.cpp`: Quản lý chu kỳ đèn giao thông.
* `TrafficRule.h/.cpp`: Xác định làn đường, quyền di chuyển, quy tắc dừng và giảm tốc.

## 3. Chu kỳ đèn

Một chu kỳ kéo dài **38 giây**, tương ứng 1.900 tick (50 tick/giây), gồm:

* Bắc – Nam đi thẳng/rẽ phải.
* Bắc – Nam rẽ trái/quay đầu.
* Đông – Tây đi thẳng/rẽ phải.
* Đông – Tây rẽ trái/quay đầu.

Giữa các pha có khoảng đèn vàng và đỏ toàn bộ giao lộ.

## 4. Quy tắc điều phối

* Đèn xanh: phương tiện được phép di chuyển.
* Đèn vàng: phương tiện chưa qua vạch dừng cần giảm tốc.
* Đèn đỏ: phương tiện phải dừng trước vạch dừng.
* Xe đã qua vạch dừng tiếp tục đi qua giao lộ, không dừng giữa giao lộ.
* Phương tiện đi theo làn quy định, không chuyển làn.

## 5. Phạm vi công việc

Phần Logic cung cấp quy tắc điều phối cho hệ thống mô phỏng; việc cập nhật vị trí, tốc độ phương tiện, giao diện và xử lý đa luồng thuộc các module khác.

## 6. Trạng thái

Đã xây dựng phiên bản ban đầu của bộ điều khiển đèn và quy tắc giao thông. Cần kiểm thử và tích hợp với các module còn lại của dự án.

## Cấu trúc thư mục

```
include/
└── Logic/
    ├── TrafficController.h
    └── TrafficRule.h

src/
└── Logic/
    ├── TrafficController.cpp
    └── TrafficRule.cpp
```
