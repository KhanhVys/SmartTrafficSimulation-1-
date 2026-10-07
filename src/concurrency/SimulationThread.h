#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

class SimulationThread {
public:
    using TickFunction = std::function<void(std::size_t)>;

    SimulationThread(TickFunction tickFunction,
                     std::chrono::milliseconds tickInterval);
    ~SimulationThread();

    SimulationThread(const SimulationThread&) = delete;
    SimulationThread& operator=(const SimulationThread&) = delete;

    void start();
    void requestStop();
    void join();
    bool isRunning() const;

private:
    void run();

    TickFunction tickFunction;
    std::chrono::milliseconds tickInterval;
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> running{false};
    std::thread worker;
};
