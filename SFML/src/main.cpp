#include "Renderer.h"
#include "concurrency/SimulationSnapshotStore.h"
#include "concurrency/SimulationThread.h" // Thêm header của SimulationThread

const unsigned int WINDOW_WIDTH = 1200;
const unsigned int WINDOW_HEIGHT = 800;

int main() {
    // Khởi tạo và chạy luồng mô phỏng nền (Thread)
    // SimulationThread simThread;
    // simThread.start();

    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, "Smart Traffic Simulation - Group 3");
    SimulationSnapshotStore& snapshotStore = SimulationSnapshotStore::getInstance();

    while (renderer.isWindowOpen()) {
        renderer.handleEvents();
        
        // Lấy snapshot thật từ luồng mô phỏng và truyền vào renderer
        const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
        renderer.render(snapshot);
    }

    // Khi người dùng đóng cửa sổ, gọi requestStop() và join() theo yêu cầu test IT-08
    // simThread.requestStop();
    // simThread.join();

    return 0;
}