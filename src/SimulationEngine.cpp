#include "SimulationEngine.h"

SimulationEngine::SimulationEngine() : currentTickTime(0.0) {
    auto northRoad = std::make_shared<Road>(Direction::North, 2);
    auto southRoad = std::make_shared<Road>(Direction::South, 2);
    auto eastRoad  = std::make_shared<Road>(Direction::East, 3);
    auto westRoad  = std::make_shared<Road>(Direction::West, 3);

    intersection.addRoad(northRoad);
    intersection.addRoad(southRoad);
    intersection.addRoad(eastRoad);
    intersection.addRoad(westRoad);

    initializeScenario();

    for (auto& v : allVehicles) {
        for (auto& r : intersection.getRoadNetwork()) {
            if (r->getDirection() == v->getDirection()) {
                r->addVehicle(v);
            }
        }
    }
}

void SimulationEngine::initializeScenario() {
    allVehicles = {
        std::make_shared<Vehicle>("CAR_N_01", Direction::North, Movement::Straight, 1, 40.0, 150.0),
        std::make_shared<Vehicle>("CAR_N_02", Direction::North, Movement::Left,     0, 35.0, 190.0),
        std::make_shared<Vehicle>("CAR_S_01", Direction::South, Movement::Straight, 1, 40.0, 160.0),
        std::make_shared<Vehicle>("CAR_S_02", Direction::South, Movement::Right,    1, 35.0, 200.0),
        std::make_shared<Vehicle>("CAR_E_01", Direction::East,  Movement::Straight, 1, 45.0, 155.0),
        std::make_shared<Vehicle>("CAR_E_02", Direction::East,  Movement::Left,     0, 30.0, 195.0),
        std::make_shared<Vehicle>("CAR_E_03", Direction::East,  Movement::UTurn,    0, 25.0, 235.0),
        std::make_shared<Vehicle>("CAR_W_01", Direction::West,  Movement::Straight, 1, 45.0, 165.0),
        std::make_shared<Vehicle>("CAR_W_02", Direction::West,  Movement::Right,    2, 35.0, 205.0),
        std::make_shared<Vehicle>("CAR_N_03", Direction::North, Movement::Straight, 1, 38.0, 230.0),
        std::make_shared<Vehicle>("CAR_S_03", Direction::South, Movement::Left,     0, 32.0, 240.0),
        std::make_shared<Vehicle>("CAR_E_04", Direction::East,  Movement::Straight, 1, 40.0, 275.0),
        std::make_shared<Vehicle>("CAR_W_03", Direction::West,  Movement::Left,     0, 30.0, 245.0)
    };
}

void SimulationEngine::step(double dt) {
    currentTickTime += dt;
    intersection.updateIntersection(dt);
}