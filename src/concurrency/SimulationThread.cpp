#include "SimulationThread.h"

#include <stdexcept>
#include <utility>

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
    std::size_t tick = 0;

    while (!stopRequested.load()) {
        tickFunction(tick++);

        std::unique_lock<std::mutex> waitLock(waitMutex);
        stopCondition.wait_for(
            waitLock,
            tickInterval,
            [this] {
                return stopRequested.load();
            }
        );
    }

    running.store(false);
}
