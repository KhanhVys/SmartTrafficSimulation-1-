#pragma once
#include <string>

class Vehicle;

class IVehicleState {
public:
    virtual ~IVehicleState() = default;
    virtual void handle(Vehicle& vehicle, double dt) = 0;
    virtual std::string getStateName() const = 0;
};

class AcceleratingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double dt) override;
    std::string getStateName() const override { return "Accelerating"; }
};

class CruisingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double dt) override;
    std::string getStateName() const override { return "Cruising"; }
};

class BrakingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double dt) override;
    std::string getStateName() const override { return "Braking"; }
};

class WaitingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double dt) override;
    std::string getStateName() const override { return "Waiting"; }
};

class TurningState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double dt) override;
    std::string getStateName() const override { return "Turning"; }
};
