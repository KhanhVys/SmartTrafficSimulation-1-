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
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true)) {
        throw std::logic_error("SimulationThread is already running");
    }

    stopRequested.store(false);
    worker = std::thread(&SimulationThread::run, this);
}

void SimulationThread::requestStop() {
    stopRequested.store(true);
}

void SimulationThread::join() {
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
        std::this_thread::sleep_for(tickInterval);
    }

    running.store(false);
}
