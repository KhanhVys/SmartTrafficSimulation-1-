#undef NDEBUG  // giữ assert hoạt động cả khi build Release

#include "integration/IntegrationCoordinator.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    IntegrationCoordinator coordinator;
    std::vector<std::string> calls;

    coordinator.addStage("core", [&calls] { calls.push_back("core"); });
    coordinator.addStage("traffic_logic", [&calls] { calls.push_back("traffic_logic"); });
    coordinator.addStage("publish_snapshot", [&calls] { calls.push_back("publish_snapshot"); });

    const auto names = coordinator.stageNames();
    assert(names.size() == 3);
    assert(names[0] == "core");

    coordinator.runOnce();
    assert(calls.size() == 3);
    assert(calls[0] == "core");
    assert(calls[1] == "traffic_logic");
    assert(calls[2] == "publish_snapshot");

    bool rejectedEmptyName = false;
    try {
        coordinator.addStage("", [] {});
    } catch (const std::invalid_argument&) {
        rejectedEmptyName = true;
    }
    assert(rejectedEmptyName);
    return 0;
}
