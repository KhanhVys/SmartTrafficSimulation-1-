#include "core/CoreSimulation.h"

#include <algorithm>
#include <limits>

#include "core/CoreConstants.h"
#include "core/TrafficGenerator.h"

CoreSimulation::CoreSimulation(int vehicleCount, double maxSpeed)
    : network(RoadNetwork::createDefault()) {
    TrafficGenerator generator(network, maxSpeed);
    vehicles = generator.generateInitialVehicles(vehicleCount);
}

void CoreSimulation::advanceClock() { ++currentTick; }

void CoreSimulation::updateTrafficLight() { trafficLight.update(currentTick); }

void CoreSimulation::updateVehicles() {
    for (const auto& vehicle : vehicles) {
        vehicle->update(TICK_SECONDS, *this);
    }
}

void CoreSimulation::step() {
    advanceClock();
    updateTrafficLight();
    updateVehicles();
}

long long CoreSimulation::getCurrentTick() const { return currentTick; }

double CoreSimulation::getSimulationSeconds() const {
    return static_cast<double>(currentTick) * TICK_SECONDS;
}

bool CoreSimulation::allVehiclesFinished() const {
    return std::all_of(vehicles.begin(), vehicles.end(),
                       [](const std::unique_ptr<Vehicle>& v) { return v->isFinished(); });
}

const std::vector<std::unique_ptr<Vehicle>>& CoreSimulation::getVehicles() const { return vehicles; }
const TrafficLight& CoreSimulation::getTrafficLight() const { return trafficLight; }
const RoadNetwork& CoreSimulation::getRoadNetwork() const { return network; }

void CoreSimulation::setMaxSpeed(double maxSpeed) {
    for (const auto& vehicle : vehicles) {
        vehicle->setMaxSpeed(maxSpeed);
    }
}

LightColor CoreSimulation::getLightColor(Direction arm, Movement movement) const {
    return trafficLight.getColor(arm, movement);
}

// Xe phía trước = xe cùng nhánh, cùng làn, ở trước mặt.
// Hai xe cùng làn nhưng khác hướng đi (vd. trái và quay đầu) chỉ chung đoạn đường vào;
// khi xe trước đã rời đoạn chung thì không còn chắn đường xe sau.
bool CoreSimulation::getLeaderGap(const Vehicle& self, double& gap) const {
    double best = std::numeric_limits<double>::max();
    for (const auto& other : vehicles) {
        if (other.get() == &self || other->isFinished()) {
            continue;
        }
        if (other->getDirection() != self.getDirection() || other->getLane() != self.getLane()) {
            continue;
        }
        if (other->getFrontPosition() <= self.getFrontPosition()) {
            continue;
        }
        const double leaderRear = other->getFrontPosition() - CAR_LENGTH;
        const bool samePath = (other->getMovement() == self.getMovement());
        if (!samePath && leaderRear >= self.getRoute().intersectionStartS) {
            continue;
        }
        best = std::min(best, leaderRear - self.getFrontPosition());
    }
    if (best == std::numeric_limits<double>::max()) {
        return false;
    }
    gap = best;
    return true;
}
