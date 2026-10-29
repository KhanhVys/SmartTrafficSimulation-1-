#include "SimulationThread.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdexcept>
#include <utility>

namespace {
#ifdef _WIN32
class WindowsTimerResolution {
public:
    WindowsTimerResolution() {
        timeBeginPeriod(1);
    }

    ~WindowsTimerResolution() {
        timeEndPeriod(1);
    }

    WindowsTimerResolution(const WindowsTimerResolution&) = delete;
    WindowsTimerResolution& operator=(const WindowsTimerResolution&) = delete;
};
#endif
} // namespace

SimulationThread::SimulationThread(TickFunction tickFunction,
                                   std::chrono::milliseconds tickInterval)
    : tickFunction(std::move(tickFunction)), tickInterval(tickInterval) {
    if (!this->tickFunction) {
        throw std::invalid_argument("tickFunction must not be empty");
    }
}

SimulationThread::~SimulationThread() {
    requestStop();
    join();
}

void SimulationThread::start() {
    std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex);

    bool expected = false;
    if (!running.compare_exchange_strong(expected, true)) {
        throw std::logic_error("SimulationThread is already running");
    }

    // Thread đã kết thúc nhưng chưa join vẫn joinable; phải join trước
    // khi gán worker mới, nếu không std::thread sẽ gọi std::terminate().
    if (worker.joinable()) {
        worker.join();
    }

    stopRequested.store(false);
    try {
        worker = std::thread(&SimulationThread::run, this);
    } catch (...) {
        running.store(false);
        throw;
    }
}

void SimulationThread::requestStop() {
    stopRequested.store(true);
    stopCondition.notify_all();
}

void SimulationThread::join() {
    std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex);
    if (worker.joinable()) {
        worker.join();
    }
}

bool SimulationThread::isRunning() const {
    return running.load();
}

void SimulationThread::run() {
#ifdef _WIN32
    // Giảm độ phân giải timer của Windows trong thời gian worker hoạt động.
    // winmm được link trong src/concurrency/CMakeLists.txt.
    const WindowsTimerResolution timerResolution;
#endif

    std::size_t tick = 0;
    const auto clock = std::chrono::steady_clock::now;
    auto nextTick = clock();

    while (!stopRequested.load()) {
        tickFunction(tick++);

        // Lập lịch theo mốc tuyệt đối để thời gian chạy callback và thời gian
        // ngủ không cộng dồn thành drift (đặc biệt trên Windows).
        nextTick += tickInterval;
        std::unique_lock<std::mutex> waitLock(waitMutex);
        stopCondition.wait_until(
            waitLock,
            nextTick,
            [this] {
                return stopRequested.load();
            }
        );
    }

    running.store(false);
}
