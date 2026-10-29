#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

#include "common/Constants.h"
#include "concurrency/SimulationThread.h"
#include "concurrency/SnapshotStore.h"
#include "core/CoreSimulation.h"
#include "integration/IntegrationCoordinator.h"
#include "integration/SnapshotAdapter.h"

#ifdef USE_GUI
#include "gui/Renderer.h"
#endif

int main() {
    auto simulation = std::make_unique<CoreSimulation>();
    SnapshotStore store;
    IntegrationCoordinator coordinator;

    coordinator.addStage("clock", [&] { simulation->advanceClock(); });
    coordinator.addStage("traffic_light", [&] { simulation->updateTrafficLight(); });
    coordinator.addStage("vehicles", [&] { simulation->updateVehicles(); });
    coordinator.addStage("publish_snapshot", [&] {
        store.publishTick(static_cast<std::size_t>(simulation->getCurrentTick()));
        store.publishLights(SnapshotAdapter::lightsOf(*simulation));
        store.publishVehicles(SnapshotAdapter::vehiclesOf(*simulation));
#ifdef USE_GUI
        if (simulation->allVehiclesFinished()) {
            simulation = std::make_unique<CoreSimulation>();
        }
#endif
    });

    SimulationThread thread([&](std::size_t) { coordinator.runOnce(); },
                            std::chrono::milliseconds(20));
    thread.start();

#ifdef USE_GUI
    Renderer renderer(1200, 800, "Smart Traffic Simulation - Group 3");
    while (renderer.isWindowOpen()) {
        renderer.handleEvents();
        renderer.render(store.getSnapshot());
    }
#else
    const std::size_t maxTicks = 3 * CYCLE_TICKS;
    SimulationSnapshot snapshot;
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        snapshot = store.getSnapshot();
        std::cout << "tick " << snapshot.tick << ": " << snapshot.vehicles.size()
                  << " xe trong ban do\n";
    } while (snapshot.tick < maxTicks && !(snapshot.tick > 0 && snapshot.vehicles.empty()));
#endif

    thread.requestStop();
    thread.join();
    return 0;
}
