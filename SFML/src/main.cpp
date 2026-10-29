#include <chrono>

#include "gui/Renderer.h"
#include "concurrency/SnapshotStore.h"
#include "concurrency/SimulationThread.h"

const unsigned int WINDOW_WIDTH = 1200;
const unsigned int WINDOW_HEIGHT = 800;

int main() {
    SnapshotStore snapshotStore;

    // TODO (hỏi Người 4): TickFunction thật cần cập nhật Core rồi publish vào store:
    //   snapshotStore.publishTick(...); publishVehicles(...); publishLights(...);
    // hoặc dùng IntegrationCoordinator. Lambda rỗng dưới đây chỉ để thread chạy được.
    SimulationThread simThread(
        [&snapshotStore]() {
            // TODO: core.update(); snapshotStore.publishTick/publishVehicles/publishLights(...)
        },
        std::chrono::milliseconds(16));
    simThread.start();

    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, "Smart Traffic Simulation - Group 3");

    while (renderer.isWindowOpen()) {
        renderer.handleEvents();

        // Lấy snapshot thật từ store và truyền vào renderer
        const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
        renderer.render(snapshot);
    }

    // Khi đóng cửa sổ: dừng và join luồng mô phỏng để an toàn luồng (test IT-08)
    simThread.requestStop();
    simThread.join();

    return 0;
}