#pragma once

// Các kiểu dùng chung cho TOÀN BỘ project (theo tài liệu giao tiếp, mục 3).
// Mọi module chỉ #include file này, KHÔNG tự định nghĩa lại các enum bên dưới.
// Muốn đổi tên hoặc thêm giá trị: báo nhóm trưởng trước.

// Hướng đường mà xe đi VÀO ngã tư.
enum class Direction { North, South, East, West };

// Hướng xe định đi sau khi vào ngã tư.
enum class Movement { Straight, Left, Right, UTurn };

// Màu đèn: chỉ là giá trị trả về để vẽ và hỏi đèn.
// Logic đèn vẫn nằm trong RedState, YellowState, GreenState.
enum class LightColor { Red, Yellow, Green };
