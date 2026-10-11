#ifndef VEHICLE_STATE_H
#define VEHICLE_STATE_H

#include <string>
#include <memory>

class Vehicle;

class IVehicleState {
public:
    virtual ~IVehicleState() = default;
    virtual void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) = 0;
    virtual std::string getStateName() const = 0;
};

class CruisingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) override;
    std::string getStateName() const override { return "Cruising"; }
};

class BrakingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) override;
    std::string getStateName() const override { return "Braking"; }
};

class WaitingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) override;
    std::string getStateName() const override { return "Waiting"; }
};

class TurningState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) override;
    std::string getStateName() const override { return "Turning"; }
};

class AcceleratingState : public IVehicleState {
public:
    void handle(Vehicle& vehicle, double distanceToNext, bool isRedLight, double dt) override;
    std::string getStateName() const override { return "Accelerating"; }
};

#endif