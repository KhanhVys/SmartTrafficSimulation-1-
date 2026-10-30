#include "core/RoadNetwork.h"

#include <cmath>
#include <stdexcept>

#include "core/CoreConstants.h"

namespace {

constexpr double TURN_CONTROL_RATIO = 0.55;
constexpr double U_TURN_CONTROL_RATIO = 0.75;

std::size_t armIndex(Direction arm) { return static_cast<std::size_t>(arm); }

bool isEastWest(Direction arm) { return arm == Direction::East || arm == Direction::West; }

// Hướng chạy của xe khi đi VÀO ngã tư từ nhánh `arm` (xe đến từ nhánh Bắc thì chạy xuống phía Nam).
Vec2 getInboundHeading(Direction arm) {
    switch (arm) {
        case Direction::North: return {0.0, 1.0};
        case Direction::South: return {0.0, -1.0};
        case Direction::East: return {-1.0, 0.0};
        case Direction::West: return {1.0, 0.0};
    }
    return {0.0, 0.0};
}

// Xe chạy bên phải đường. Trục y hướng xuống nên bên phải của (x, y) là (-y, x).
Vec2 rotateRight(Vec2 heading) { return {-heading.y, heading.x}; }
Vec2 rotateLeft(Vec2 heading) { return {heading.y, -heading.x}; }

// Nhánh mà xe rời ngã tư để đi ra khi chạy theo hướng `heading`.
Direction getArmFromHeading(Vec2 heading) {
    if (heading.y > 0.5) return Direction::South;
    if (heading.y < -0.5) return Direction::North;
    if (heading.x > 0.5) return Direction::East;
    return Direction::West;
}

Vec2 cubicBezier(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, double t) {
    const double u = 1.0 - t;
    return p0 * (u * u * u) + p1 * (3.0 * u * u * t) + p2 * (3.0 * u * t * t) + p3 * (t * t * t);
}

}  // namespace

RoadNetwork RoadNetwork::createDefault() {
    RoadNetwork network;
    network.addComponent(std::make_shared<Road>(Direction::North, NS_LANE_COUNT, NS_LANE_WIDTH,
                                                NS_CENTER_HALF_GAP, NS_ROAD_LENGTH));
    network.addComponent(std::make_shared<Road>(Direction::South, NS_LANE_COUNT, NS_LANE_WIDTH,
                                                NS_CENTER_HALF_GAP, NS_ROAD_LENGTH));
    network.addComponent(std::make_shared<Road>(Direction::East, EW_LANE_COUNT, EW_LANE_WIDTH,
                                                EW_MEDIAN_HALF_WIDTH, EW_ROAD_LENGTH));
    network.addComponent(std::make_shared<Road>(Direction::West, EW_LANE_COUNT, EW_LANE_WIDTH,
                                                EW_MEDIAN_HALF_WIDTH, EW_ROAD_LENGTH));
    network.addComponent(std::make_shared<Intersection>(network.getRoad(Direction::North).getHalfWidth(),
                                                        network.getRoad(Direction::East).getHalfWidth()));
    return network;
}

std::string RoadNetwork::getName() const { return "RoadNetwork"; }

int RoadNetwork::getCapacity() const {
    int total = 0;
    for (const auto& component : components) {
        total += component->getCapacity();
    }
    return total;
}

void RoadNetwork::addComponent(std::shared_ptr<IRoadComponent> component) {
    if (!component) {
        throw std::invalid_argument("RoadNetwork::addComponent nhan con tro rong");
    }
    if (auto road = std::dynamic_pointer_cast<Road>(component)) {
        roads[armIndex(road->getArm())] = road;
    } else if (auto crossing = std::dynamic_pointer_cast<Intersection>(component)) {
        intersection = crossing;
    }
    components.push_back(std::move(component));
}

std::vector<std::shared_ptr<IRoadComponent>> RoadNetwork::getConnectedComponents() const {
    return components;
}

const Road& RoadNetwork::getRoad(Direction arm) const {
    const auto& road = roads[armIndex(arm)];
    if (!road) {
        throw std::logic_error("RoadNetwork chua co nhanh duong nay");
    }
    return *road;
}

const Intersection& RoadNetwork::getIntersection() const {
    if (!intersection) {
        throw std::logic_error("RoadNetwork chua co nga tu");
    }
    return *intersection;
}

bool RoadNetwork::isMovementAllowed(Direction arm, int lane, Movement movement) const {
    const Road& road = getRoad(arm);
    if (lane < 0 || lane >= road.getLaneCount()) {
        return false;
    }
    const bool leftGroup = (movement == Movement::Left || movement == Movement::UTurn);
    if (lane == 0) {
        return leftGroup;
    }
    if (isEastWest(arm)) {
        return (lane == 1 && movement == Movement::Straight) ||
               (lane == 2 && movement == Movement::Right);
    }
    return movement == Movement::Straight || movement == Movement::Right;
}

Route RoadNetwork::buildRoute(Direction arm, int lane, Movement movement) const {
    if (!isMovementAllowed(arm, lane, movement)) {
        throw std::invalid_argument("To hop nhanh/lan/huong di khong hop le");
    }

    const Road& inRoad = getRoad(arm);
    const Vec2 headingIn = getInboundHeading(arm);

    Vec2 headingOut = headingIn;
    switch (movement) {
        case Movement::Straight: headingOut = headingIn; break;
        case Movement::Right: headingOut = rotateRight(headingIn); break;
        case Movement::Left: headingOut = rotateLeft(headingIn); break;
        case Movement::UTurn: headingOut = headingIn * -1.0; break;
    }
    const Road& outRoad = getRoad(getArmFromHeading(headingOut));

    // Làn ra: đi thẳng giữ nguyên làn, rẽ phải vào làn ngoài cùng, rẽ trái/quay đầu vào làn trong cùng.
    int outLane = 0;
    if (movement == Movement::Straight) {
        outLane = lane;
    } else if (movement == Movement::Right) {
        outLane = outRoad.getLaneCount() - 1;
    }

    const Intersection& box = getIntersection();
    const auto boxHalfAlong = [&box](Vec2 heading) {
        return std::abs(heading.x) > 0.5 ? box.getHalfWidth() : box.getHalfHeight();
    };

    const Vec2 center{WORLD_CENTER_X, WORLD_CENTER_Y};
    const Vec2 boxEntry = center - headingIn * boxHalfAlong(headingIn) +
                          rotateRight(headingIn) * inRoad.getLaneCenterOffset(lane);
    const Vec2 boxExit = center + headingOut * boxHalfAlong(headingOut) +
                         rotateRight(headingOut) * outRoad.getLaneCenterOffset(outLane);
    const Vec2 spawn = boxEntry - headingIn * inRoad.getLength();
    const Vec2 end = boxExit + headingOut * outRoad.getLength();

    // Đường cong rẽ: Bezier bậc 3, tiếp tuyến trùng hướng vào ở đầu và hướng ra ở cuối.
    const Vec2 chord = boxExit - boxEntry;
    double controlIn = 0.0;
    double controlOut = 0.0;
    if (movement == Movement::UTurn) {
        const double separation = std::abs(dot(chord, rotateRight(headingIn)));
        controlIn = controlOut = U_TURN_CONTROL_RATIO * separation;
    } else {
        controlIn = TURN_CONTROL_RATIO * std::abs(dot(chord, headingIn));
        controlOut = TURN_CONTROL_RATIO * std::abs(dot(chord, headingOut));
    }
    const Vec2 control1 = boxEntry + headingIn * controlIn;
    const Vec2 control2 = boxExit - headingOut * controlOut;

    Route route;
    route.path.addPoint(spawn);
    for (int i = 0; i <= BEZIER_SEGMENTS; ++i) {
        const double t = static_cast<double>(i) / BEZIER_SEGMENTS;
        route.path.addPoint(cubicBezier(boxEntry, control1, control2, boxExit, t));
        if (i == 0) {
            route.intersectionStartS = route.path.getLength();
        }
    }
    route.intersectionEndS = route.path.getLength();
    route.path.addPoint(end);
    route.stopLineS = route.intersectionStartS - STOP_LINE_GAP;
    return route;
}
