#include "core/TrafficGenerator.h"

#include <iterator>
#include <stdexcept>

#include "common/Constants.h"
#include "core/CoreConstants.h"
#include <map>
#include <random>
#include <utility>
#include <vector>
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

std::vector<std::unique_ptr<Vehicle>> TrafficGenerator::generateRandomVehicles(int count, unsigned seed) const {
    if (count < 0 || count > NUM_VEHICLES) {
        throw std::invalid_argument("So xe ban dau phai nam trong [0, NUM_VEHICLES]");
    }
    std::mt19937 rng(seed != 0 ? seed : std::random_device{}());

    // 1. Liệt kê mọi tổ hợp (nhánh, làn, hướng đi) mà luật cho phép
    struct Option { Direction arm; int lane; Movement movement; };
    std::vector<Option> options;
    for (Direction arm : {Direction::North, Direction::South, Direction::East, Direction::West}) {
        const int lanes = network.getRoad(arm).getLaneCount();
        for (int lane = 0; lane < lanes; ++lane) {
            for (Movement m : {Movement::Straight, Movement::Left, Movement::Right, Movement::UTurn}) {
                if (network.isMovementAllowed(arm, lane, m)) options.push_back({arm, lane, m});
            }
        }
    }

    // 2. Bốc ngẫu nhiên từng xe. Mỗi làn chỉ chứa tối đa vài xe xếp hàng (tùy độ dài đường)
    std::map<std::pair<int, int>, int> queued;   // (nhánh, làn) -> số xe đã đặt
    std::uniform_int_distribution<std::size_t> pick(0, options.size() - 1);
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    vehicles.reserve(static_cast<std::size_t>(count));

    for (int i = 0; i < count; ++i) {
        for (;;) {
            const Option& o = options[pick(rng)];
            const std::pair<int, int> key{static_cast<int>(o.arm), o.lane};
            const double length = network.getRoad(o.arm).getLength();
            const int maxQueue = static_cast<int>((length - STOP_LINE_GAP - CAR_LENGTH - 40.0 - 60.0) / 100.0) + 1;
            if (queued[key] >= maxQueue) continue;       // làn này đã đầy, bốc lại
            const int q = queued[key]++;
            // cùng công thức khoảng cách như kịch bản cố định: xe cùng làn cách nhau >= 60px
            const double distance = 60.0 + 100.0 * q + 20.0 * (i % 3);
            vehicles.push_back(generateVehicle(i + 1, o.arm, o.lane, o.movement, distance));
            break;
        }
    }
    return vehicles;
}