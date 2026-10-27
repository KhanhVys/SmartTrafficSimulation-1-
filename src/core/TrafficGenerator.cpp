#include "core/TrafficGenerator.h"

#include <iterator>
#include <stdexcept>

#include "common/Constants.h"
#include "core/CoreConstants.h"

namespace {

struct SpawnSpec {
    Direction arm;
    int lane;
    Movement movement;
    int queueIndex;  // thứ tự trong hàng cùng làn: 0 = gần vạch nhất
};

// Đường đôi Đông-Tây: lane 0 = trái/quay đầu, 1 = thẳng, 2 = phải.
// Đường đơn Bắc-Nam:  lane 0 = trái/quay đầu, 1 = thẳng + phải.
constexpr SpawnSpec SCENARIO[] = {
    {Direction::West, 1, Movement::Straight, 0},
    {Direction::West, 1, Movement::Straight, 1},
    {Direction::West, 2, Movement::Right, 0},
    {Direction::West, 0, Movement::Left, 0},
    {Direction::East, 1, Movement::Straight, 0},
    {Direction::East, 0, Movement::UTurn, 0},
    {Direction::East, 2, Movement::Right, 0},
    {Direction::North, 1, Movement::Straight, 0},
    {Direction::North, 0, Movement::Left, 0},
    {Direction::North, 1, Movement::Right, 1},
    {Direction::South, 1, Movement::Straight, 0},
    {Direction::South, 0, Movement::UTurn, 0},
    {Direction::South, 1, Movement::Right, 1},
};

constexpr int SCENARIO_SIZE = static_cast<int>(std::size(SCENARIO));
static_assert(SCENARIO_SIZE == NUM_VEHICLES, "Kich ban phai co dung NUM_VEHICLES xe");

}  // namespace

TrafficGenerator::TrafficGenerator(const RoadNetwork& network, double maxSpeed)
    : network(network), maxSpeed(maxSpeed) {}

std::unique_ptr<Vehicle> TrafficGenerator::generateVehicle(int vehicleId, Direction arm, int lane,
                                                           Movement movement,
                                                           double distanceToStopLine) const {
    Route route = network.buildRoute(arm, lane, movement);
    const double startFront = route.stopLineS - distanceToStopLine;
    if (startFront < CAR_LENGTH) {
        throw std::invalid_argument("Vi tri sinh xe nam ngoai ban do");
    }
    return std::make_unique<Vehicle>(vehicleId, arm, lane, movement, std::move(route), startFront, maxSpeed);
}

std::vector<std::unique_ptr<Vehicle>> TrafficGenerator::generateInitialVehicles(int count) const {
    if (count < 0 || count > SCENARIO_SIZE) {
        throw std::invalid_argument("So xe ban dau phai nam trong [0, 13]");
    }
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    vehicles.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        const SpawnSpec& spec = SCENARIO[i];
        // Các xe cùng làn cách nhau >= 60px giữa hai đầu xe, đủ chỗ cho xe dài 40px + khoảng hở.
        const double distance = 60.0 + 100.0 * spec.queueIndex + 20.0 * (i % 3);
        vehicles.push_back(generateVehicle(i + 1, spec.arm, spec.lane, spec.movement, distance));
    }
    return vehicles;
}
