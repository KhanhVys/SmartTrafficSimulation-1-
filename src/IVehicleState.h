#pragma once
class Vehicle;

class IVehicleState {
public:
    virtual ~IVehicleState() = default;
    virtual void update(Vehicle& vehicle, float deltaTime) = 0;
    virtual const char* getName() const = 0;
};