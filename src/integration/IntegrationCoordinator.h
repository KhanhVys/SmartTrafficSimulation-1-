#pragma once

#include <functional>
#include <string>
#include <vector>

// Người 4: điều phối các module thông qua callback được truyền từ bên ngoài.
// File này KHÔNG include Vehicle/TrafficLight/SimulationThread để tránh phụ thuộc
// vào code đang được các thành viên khác chỉnh sửa.
class IntegrationCoordinator {
public:
    using Step = std::function<void()>;

    struct Stage {
        std::string name;
        Step run;
    };

    // Đăng ký một bước tích hợp theo thứ tự cần chạy.
    void addStage(std::string name, Step run);

    // Chạy lần lượt các bước đã đăng ký.
    // Ném std::logic_error nếu có bước chưa được gán callback.
    void runOnce() const;

    // Trả về tên các bước theo đúng thứ tự đã đăng ký (hữu ích cho kiểm tra/log).
    std::vector<std::string> stageNames() const;

private:
    std::vector<Stage> stages;
};
