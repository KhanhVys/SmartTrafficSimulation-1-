#include "SimulationEngine.h"
#include <iostream>

SimulationEngine::SimulationEngine() : currentSimulationTime(0.0) {
    roads.push_back(std::make_shared<Road>(Direction::North, 2));
    roads.push_back(std::make_shared<Road>(Direction::South, 2));
    roads.push_back(std::make_shared<Road>(Direction::East, 3));
    roads.push_back(std::make_shared<Road>(Direction::West, 3));

    initializeScenario();
}

void SimulationEngine::initializeScenario() {
    auto v1 = std::make_shared<Vehicle>("CAR_N_1", Direction::North, TurnType::Straight, 1, 40.0);
    auto v2 = std::make_shared<Vehicle>("CAR_N_2", Direction::North, TurnType::Left, 0, 35.0);
    auto v3 = std::make_shared<Vehicle>("CAR_S_1", Direction::South, TurnType::Straight, 1, 40.0);
    auto v4 = std::make_shared<Vehicle>("CAR_S_2", Direction::South, TurnType::Right, 1, 35.0);
    auto v5 = std::make_shared<Vehicle>("CAR_E_1", Direction::East, TurnType::Straight, 1, 45.0);
    auto v6 = std::make_shared<Vehicle>("CAR_E_2", Direction::East, TurnType::Left, 0, 30.0);
    auto v7 = std::make_shared<Vehicle>("CAR_E_3", Direction::East, TurnType::UTurn, 0, 25.0);
    auto v8 = std::make_shared<Vehicle>("CAR_W_1", Direction::West, TurnType::Straight, 1, 45.0);
    auto v9 = std::make_shared<Vehicle>("CAR_W_2", Direction::West, TurnType::Right, 2, 35.0);
    auto v10 = std::make_shared<Vehicle>("CAR_N_3", Direction::North, TurnType::Straight, 1, 38.0);
    auto v11 = std::make_shared<Vehicle>("CAR_S_3", Direction::South, TurnType::Left, 0, 32.0);
    auto v12 = std::make_shared<Vehicle>("CAR_E_4", Direction::East, TurnType::Straight, 1, 40.0);
    auto v13 = std::make_shared<Vehicle>("CAR_W_3", Direction::West, TurnType::Left, 0, 30.0);

    allVehicles = {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13};

    for (auto& v : allVehicles) {
        for (auto& r : roads) {
            if (r->getDirection() == v->getDirection()) {
                r->addVehicle(v);
            }
        }
    }
}

void SimulationEngine::step(double dt) {
    currentSimulationTime += dt;
    trafficLight.update(dt);

    for (auto& v : allVehicles) {
        bool red = trafficLight.isRedFor(v->getDirection(), v->getTurnType());
        v->update(dt, v->getPosition(), red, true);
    }
}