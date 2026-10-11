#include "IntegrationCoordinator.h"

#include <stdexcept>
#include <utility>

void IntegrationCoordinator::addStage(std::string name, Step run) {
    if (name.empty()) {
        throw std::invalid_argument("Integration stage name must not be empty");
    }
    stages.push_back({std::move(name), std::move(run)});
}

void IntegrationCoordinator::runOnce() const {
    for (const auto& stage : stages) {
        if (!stage.run) {
            throw std::logic_error("Integration stage has no callback: " + stage.name);
        }
        stage.run();
    }
}

std::vector<std::string> IntegrationCoordinator::stageNames() const {
    std::vector<std::string> names;
    names.reserve(stages.size());
    for (const auto& stage : stages) {
        names.push_back(stage.name);
    }
    return names;
}
