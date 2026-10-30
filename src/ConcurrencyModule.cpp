#include <atomic>
#include <chrono>
#include <cstddef>
#include "common/TurnCurve.h"
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

enum class LightColor { Red, Yellow, Green };
enum class Direction { North, South, East, West };
enum class Movement { Straight, Left, Right, UTurn };

struct VehicleSnapshot {
    int vehicleId = 0;
    double x = 0.0;
    double y = 0.0;
    Direction direction = Direction::North;
    Movement movement = Movement::Straight;
    bool signalingLeft = false;
    // Góc xoay hiển thị của xe, đơn vị độ; xe quay đầu thường là 180 độ.
    double rotationAngleDegrees = 0.0;
};

struct LightSnapshot {
    Direction direction = Direction::North;
    bool turnArrow = false;
    LightColor color = LightColor::Red;
};

struct SimulationSnapshot {
    std::size_t tick = 0;
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};

class SnapshotStore {
public:
    void publishSnapshot(
        std::size_t tick,
        const std::vector<LightSnapshot>& newLights,
        const std::vector<VehicleSnapshot>& newVehicles) {
        // Thứ tự khóa bắt buộc: lightMutex trước, vehicleMutex sau.
        std::lock_guard<std::mutex> lightLock(lightMutex);
        std::lock_guard<std::mutex> vehicleLock(vehicleMutex);

        lights = newLights;
        vehicles = newVehicles;
        currentTick.store(tick);
    }

    SimulationSnapshot getSnapshot() const {
        SimulationSnapshot snapshot;

        // Quy tắc cố định khi cần lấy cả hai vùng dữ liệu:
        // lightMutex trước, vehicleMutex sau để tránh deadlock.
        std::lock_guard<std::mutex> lightLock(lightMutex);
        std::lock_guard<std::mutex> vehicleLock(vehicleMutex);

        snapshot.tick = currentTick.load();
        snapshot.lights = lights;
        snapshot.vehicles = vehicles;

        // GUI nhận bản sao và không giữ khóa trong lúc render.
        return snapshot;
    }

private:
    mutable std::mutex vehicleMutex;
    mutable std::mutex lightMutex;
    std::atomic<std::size_t> currentTick{0};
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};

class SimulationThread {
public:
    using TickFunction = std::function<void(std::size_t)>;

    SimulationThread(TickFunction tickFunction,
                     std::chrono::milliseconds tickInterval)
        : tickFunction(std::move(tickFunction)), tickInterval(tickInterval) {
        if (!this->tickFunction) {
            throw std::invalid_argument("tickFunction must not be empty");
        }
    }

    ~SimulationThread() {
        requestStop();
        join();
    }

    SimulationThread(const SimulationThread&) = delete;
    SimulationThread& operator=(const SimulationThread&) = delete;

    void start() {
        bool expected = false;
        if (!running.compare_exchange_strong(expected, true)) {
            throw std::logic_error("SimulationThread is already running");
        }

        stopRequested.store(false);
        worker = std::thread(&SimulationThread::run, this);
    }

    void requestStop() {
        stopRequested.store(true);
    }

    void join() {
        if (worker.joinable()) {
            worker.join();
        }
    }

    bool isRunning() const {
        return running.load();
    }

private:
    void run() {
        std::size_t tick = 0;

        while (!stopRequested.load()) {
            tickFunction(tick++);
            std::this_thread::sleep_for(tickInterval);
        }

        running.store(false);
    }

    TickFunction tickFunction;
    std::chrono::milliseconds tickInterval;
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> running{false};
    std::thread worker;
};

int main() {
    SnapshotStore snapshotStore;

    // Trong project thật, callback này sẽ gọi Core của Người 1
    // và Logic giao thông của Người 2.
    auto updateSimulation = [&snapshotStore](std::size_t tick) {
        const LightColor color = (tick % 2 == 0)
                                     ? LightColor::Green
                                     : LightColor::Red;
        const double turnProgress =
            static_cast<double>(tick % 50) / 49.0;
        const TurnCurveSample turnSample = TurnCurve::sampleQuadraticBezier(
            {100.0, 300.0},
            {240.0, 190.0},
            {100.0, 100.0},
            turnProgress
        );

        snapshotStore.publishSnapshot(
            tick,
            {
                {Direction::North, false, color},
                {Direction::South, true, color},
            },
            {
                {1, static_cast<double>(tick), 0.0,
                 Direction::North, Movement::Straight, false, 0.0},
                {2, 0.0, static_cast<double>(tick),
                 Direction::East, Movement::Right, true, 90.0},
                {3, turnSample.position.x, turnSample.position.y,
                 Direction::South, Movement::UTurn,
                 true, turnSample.rotationAngleDegrees},
            }
        );
    };

    SimulationThread simulationThread(
        updateSimulation,
        std::chrono::milliseconds(20));
    simulationThread.start();

    // Luồng chính đại diện cho vòng lặp SFML của Người 5.
    for (int frame = 0; frame < 5; ++frame) {
        const SimulationSnapshot snapshot = snapshotStore.getSnapshot();

        std::cout << "frame=" << frame
                  << " tick=" << snapshot.tick
                  << " vehicles=" << snapshot.vehicles.size()
                  << " lights=" << snapshot.lights.size() << '\n';
        for (const VehicleSnapshot& vehicle : snapshot.vehicles) {
            std::cout << "  vehicle " << vehicle.vehicleId
                      << " rotation=" << vehicle.rotationAngleDegrees
                      << " degrees\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    simulationThread.requestStop();
    simulationThread.join();
    std::cout << "simulation thread stopped safely\n";
    return 0;
}
