#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
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
    void rethrowWorkerException();

    TickFunction tickFunction;
    std::chrono::milliseconds tickInterval;
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> running{false};
    mutable std::mutex lifecycleMutex;
    std::condition_variable stopCondition;
    std::mutex waitMutex;
    std::mutex exceptionMutex;
    std::exception_ptr workerException;
    std::thread worker;
};
