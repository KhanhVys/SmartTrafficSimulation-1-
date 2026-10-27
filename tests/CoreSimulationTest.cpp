#undef NDEBUG  // giữ kiểm tra hoạt động cả khi build Release

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <set>
#include <stdexcept>
#include <vector>
#include <string>

#include "common/Constants.h"
#include "core/CoreConstants.h"
#include "core/CoreSimulation.h"
#include "core/TrafficLight.h"

namespace {

int failures = 0;

#define CHECK(cond, ...)                                              \
    do {                                                              \
        if (!(cond)) {                                                \
            ++failures;                                               \
            std::printf("  FAIL [%s:%d] %s | ", __FILE__, __LINE__, #cond); \
            std::printf(__VA_ARGS__);                                 \
            std::printf("\n");                                        \
        }                                                             \
    } while (0)

const char* colorName(LightColor c) {
    return c == LightColor::Red ? "Red" : (c == LightColor::Yellow ? "Yellow" : "Green");
}

// Màu mong đợi lấy thẳng từ bảng trong README (độc lập với code đang test).
LightColor expectedColor(LightGroup group, int second) {
    int greenFrom = 0, greenTo = 0;
    switch (group) {
        case LightGroup::NorthSouthStraight: greenFrom = 0; greenTo = 5; break;
        case LightGroup::NorthSouthTurn: greenFrom = 10; greenTo = 14; break;
        case LightGroup::EastWestStraight: greenFrom = 19; greenTo = 24; break;
        case LightGroup::EastWestTurn: greenFrom = 29; greenTo = 33; break;
    }
    if (second >= greenFrom && second <= greenTo) return LightColor::Green;
    if (second > greenTo && second <= greenTo + 2) return LightColor::Yellow;
    return LightColor::Red;
}

void testLightCycle() {
    std::printf("[1] Chu ky den 38 giay (IT-05)\n");
    TrafficLight light;
    const LightGroup groups[] = {LightGroup::NorthSouthStraight, LightGroup::NorthSouthTurn,
                                 LightGroup::EastWestStraight, LightGroup::EastWestTurn};
    for (long long tick = 0; tick < 2 * CYCLE_TICKS; ++tick) {
        light.update(tick);
        const int second = static_cast<int>((tick % CYCLE_TICKS) / TICKS_PER_SECOND);
        int notRed = 0;
        for (LightGroup g : groups) {
            const LightColor got = light.getGroupColor(g);
            CHECK(got == expectedColor(g, second), "tick=%lld group=%d got=%s", tick,
                  static_cast<int>(g), colorName(got));
            if (got != LightColor::Red) ++notRed;
        }
        CHECK(notRed <= 1, "tick=%lld co %d nhom khong do cung luc", tick, notRed);
    }
    CHECK(TrafficLight::getGroupOf(Direction::North, Movement::Right) == LightGroup::NorthSouthStraight, "ret phai chung den thang");
    CHECK(TrafficLight::getGroupOf(Direction::East, Movement::UTurn) == LightGroup::EastWestTurn, "quay dau chung den re trai");
}

void testRoutes() {
    std::printf("[2] Lo trinh hop le, lien tuc, nam trong ban do\n");
    const RoadNetwork net = RoadNetwork::createDefault();
    const Direction arms[] = {Direction::North, Direction::South, Direction::East, Direction::West};
    const Movement moves[] = {Movement::Straight, Movement::Left, Movement::Right, Movement::UTurn};
    int valid = 0;
    for (Direction arm : arms) {
        for (int lane = 0; lane < net.getRoad(arm).getLaneCount(); ++lane) {
            for (Movement m : moves) {
                if (!net.isMovementAllowed(arm, lane, m)) continue;
                ++valid;
                const Route r = net.buildRoute(arm, lane, m);
                CHECK(r.stopLineS < r.intersectionStartS && r.intersectionStartS < r.intersectionEndS, "moc s sai");
                const double len = r.path.getLength();
                for (double s = 0.0; s + 1.0 < len; s += 1.0) {
                    const Vec2 a = r.path.getPositionAt(s), b = r.path.getPositionAt(s + 1.0);
                    CHECK(length(b - a) < 1.2, "duong di khong lien tuc arm=%d lane=%d m=%d s=%.0f", (int)arm, lane, (int)m, s);
                }
                const Vec2 p0 = r.path.getPositionAt(0), p1 = r.path.getPositionAt(len);
                CHECK(p0.x >= 0 && p0.x <= WORLD_WIDTH && p0.y >= 0 && p0.y <= WORLD_HEIGHT, "diem sinh ngoai ban do");
                CHECK(p1.x >= 0 && p1.x <= WORLD_WIDTH && p1.y >= 0 && p1.y <= WORLD_HEIGHT, "diem cuoi ngoai ban do");
            }
        }
    }
    // Đường đôi: 3 làn x 2 nhánh x (làn trái 2 hướng + thẳng + phải = 4) = 8; đường đơn: 2 nhánh x (2 + 2) = 8
    CHECK(valid == 16, "so to hop hop le = %d", valid);
    CHECK(!net.isMovementAllowed(Direction::West, 1, Movement::Left), "lan giua khong duoc re trai");
    bool threw = false;
    try { net.buildRoute(Direction::North, 0, Movement::Straight); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw, "phai nem invalid_argument");
    CHECK(net.getCapacity() > 0 && net.getConnectedComponents().size() == 5, "composite");
    bool leafThrew = false;
    Road leafRoad(Direction::North, NS_LANE_COUNT, NS_LANE_WIDTH, NS_CENTER_HALF_GAP, NS_ROAD_LENGTH);
    try { leafRoad.addComponent(nullptr); } catch (const std::logic_error&) { leafThrew = true; }
    CHECK(leafThrew, "la khong duoc chua con");
}

void testSimulation(bool print) {
    std::printf("[3] Chay mo phong day du 13 xe (IT-02, IT-03, IT-04, IT-10)\n");
    CoreSimulation sim;
    CHECK(sim.getVehicles().size() == static_cast<std::size_t>(NUM_VEHICLES), "so xe = %zu", sim.getVehicles().size());

    std::set<std::string> statesSeen;
    const std::size_t n = sim.getVehicles().size();
    std::vector<double> prevFront(n), prevX(n), prevY(n);
    std::vector<bool> crossed(n, false);
    double minGap = 1e9, maxJump = 0.0;
    int crossedOnYellow = 0;
    const long long maxTicks = 120 * TICKS_PER_SECOND;

    for (std::size_t i = 0; i < n; ++i) {
        const Vehicle& v = *sim.getVehicles()[i];
        prevFront[i] = v.getFrontPosition();
        prevX[i] = v.getPosition().x;
        prevY[i] = v.getPosition().y;
    }

    while (sim.getCurrentTick() < maxTicks && !sim.allVehiclesFinished()) {
        sim.step();
        for (std::size_t i = 0; i < n; ++i) {
            const Vehicle& v = *sim.getVehicles()[i];
            statesSeen.insert(v.getStateName());
            if (v.isFinished()) continue;

            // Xe chưa qua vạch mà đèn đỏ thì không được vượt vạch (IT-03).
            if (!crossed[i] && v.getFrontPosition() > v.getRoute().stopLineS + LINE_CROSS_TOLERANCE) {
                crossed[i] = true;
                const LightColor c = sim.getTrafficLight().getColor(v.getDirection(), v.getMovement());
                CHECK(c != LightColor::Red, "xe %d vuot vach luc den DO, giay %.2f", v.getId(), sim.getSimulationSeconds());
                if (c == LightColor::Yellow) ++crossedOnYellow;
            }
            // Đã qua vạch thì không được đứng chờ (IT-04).
            if (v.getFrontPosition() > v.getRoute().stopLineS + 1.0) {
                CHECK(std::strcmp(v.getStateName(), "WaitingState") != 0, "xe %d dung cho sau vach, giay %.2f", v.getId(), sim.getSimulationSeconds());
            }
            CHECK(v.getSpeed() <= v.getMaxSpeed() + 1e-6 && v.getSpeed() >= 0.0, "toc do xe %d = %.2f", v.getId(), v.getSpeed());
            CHECK(v.getFrontPosition() >= prevFront[i] - 1e-9, "xe %d di lui", v.getId());

            const Vec2 p = v.getPosition();
            maxJump = std::max(maxJump, std::hypot(p.x - prevX[i], p.y - prevY[i]));
            prevX[i] = p.x; prevY[i] = p.y; prevFront[i] = v.getFrontPosition();

            double gap = 0.0;
            if (sim.getLeaderGap(v, gap)) {
                minGap = std::min(minGap, gap);
                CHECK(gap >= 0.0, "xe %d de len xe truoc, gap=%.2f giay %.2f", v.getId(), gap, sim.getSimulationSeconds());
            }
        }
        if (print && sim.getCurrentTick() % TICKS_PER_SECOND == 0) {
            std::printf("t=%5.1fs |", sim.getSimulationSeconds());
            for (const auto& v : sim.getVehicles()) {
                std::printf(" %2d:%c%-3.0f", v->getId(), v->getStateName()[0], v->getSpeed());
            }
            std::printf("\n");
        }
    }

    CHECK(sim.allVehiclesFinished(), "con xe chua ra khoi ban do sau %.0f giay", sim.getSimulationSeconds());
    CHECK(maxJump <= DEFAULT_SPEED * TICK_SECONDS + 0.5, "vi tri nhay %.2f px/tick", maxJump);
    for (const char* s : {"AcceleratingState", "BrakingState", "WaitingState", "TurningState", "CruisingState"}) {
        CHECK(statesSeen.count(s) == 1, "chua tung vao %s", s);
    }
    std::printf("    xong luc %.1f giay | khoang hep nho nhat giua 2 xe cung lan: %.1f px | vuot vach luc vang: %d xe\n",
                sim.getSimulationSeconds(), minGap, crossedOnYellow);
}

void testCustomSpeed() {
    std::printf("[4] Toc do tuy chinh duoc\n");
    CoreSimulation slow(NUM_VEHICLES, 40.0);
    CoreSimulation fast(NUM_VEHICLES, 150.0);
    while (slow.getCurrentTick() < 200 * TICKS_PER_SECOND && !slow.allVehiclesFinished()) slow.step();
    while (fast.getCurrentTick() < 200 * TICKS_PER_SECOND && !fast.allVehiclesFinished()) fast.step();
    CHECK(slow.allVehiclesFinished() && fast.allVehiclesFinished(), "xe cham/nhanh deu phai chay xong");
    for (const auto& v : slow.getVehicles()) CHECK(v->getMaxSpeed() == 40.0, "max speed");
    std::printf("    cham xong luc %.1fs, nhanh xong luc %.1fs\n", slow.getSimulationSeconds(), fast.getSimulationSeconds());
}

}  // namespace

int main(int argc, char** argv) {
    const bool print = (argc > 1 && std::strcmp(argv[1], "--print") == 0);
    testLightCycle();
    testRoutes();
    testSimulation(print);
    testCustomSpeed();
    if (failures == 0) {
        std::printf("TAT CA TEST DAT\n");
        return 0;
    }
    std::printf("%d LOI\n", failures);
    return 1;
}
