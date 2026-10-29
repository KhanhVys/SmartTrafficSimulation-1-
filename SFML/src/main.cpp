#include "gui/Renderer.h"
#include "concurrency/SnapshotStore.h"
#include "concurrency/SimulationThread.h" // Hoặc Header khởi tạo luồng của Người 3 / Người 4

const unsigned int WINDOW_WIDTH = 1200;
const unsigned int WINDOW_HEIGHT = 800;

int main() {
    // Khởi tạo SnapshotStore và SimulationThread theo kiến trúc của Người 3 & Người 4
    SnapshotStore snapshotStore;
    // SimulationThread simThread(snapshotStore); // Truyền store vào thread/engine nếu cần
    // simThread.start();

    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, "Smart Traffic Simulation - Group 3");

    while (renderer.isWindowOpen()) {
        renderer.handleEvents();
        
        // Lấy snapshot thật từ store và truyền vào renderer
        const SimulationSnapshot snapshot = snapshotStore.getSnapshot();
        renderer.render(snapshot);
    }

    // Khi đóng cửa sổ, gọi requestStop() và join() để đảm bảo an toàn luồng (test IT-08)
    // simThread.requestStop();
    // simThread.join();

    return 0;
}