#pragma once
#include "Vehicle.h"
#include <memory>

class IVehicleState {
public:
    virtual ~IVehicleState() = default;
    virtual void update(Vehicle& vehicle, float deltaTime) = 0;
    virtual const char* getName() const = 0;
};

class AcceleratingState : public IVehicleState {
public:
    void update(Vehicle& v, float dt) override;
    const char* getName() const override { return "Accelerating"; }
};

class BrakingState : public IVehicleState {
public:
    void update(Vehicle& v, float dt) override;
    const char* getName() const override { return "Braking"; }
};

class WaitingState : public IVehicleState {
public:
    void update(Vehicle& v, float dt) override;
    const char* getName() const override { return "Waiting"; }
};

class CruisingState : public IVehicleState {
public:
    void update(Vehicle& v, float dt) override;
    const char* getName() const override { return "Cruising"; }
};

class TurningState : public IVehicleState {
public:
    void update(Vehicle& v, float dt) override;
    const char* getName() const override { return "Turning"; }
};

// --- INLINE IMPLEMENTATIONS ---
inline void AcceleratingState::update(Vehicle& v, float dt) {
    if (v.shouldBrake()) { v.setState(std::make_unique<BrakingState>()); return; }
    v.setSpeed(v.getSpeed() + v.getAcceleration() * dt);
    if (v.getSpeed() >= v.getMaxSpeed()) {
        v.setSpeed(v.getMaxSpeed());
        v.setState(std::make_unique<CruisingState>());
        return;
    }
    v.moveForward(dt);
}

inline void BrakingState::update(Vehicle& v, float dt) {
    if (!v.shouldBrake()) { v.setState(std::make_unique<AcceleratingState>()); return; }
    
    v.setSpeed(v.getSpeed() - v.getDeceleration() * dt);
    if (v.getSpeed() <= 0.0f) {
        v.setSpeed(0.0f);
        v.setState(std::make_unique<WaitingState>());
        return;
    }
    v.moveForward(dt);
}

inline void WaitingState::update(Vehicle& v, float dt) {
    (void)dt;
    if (v.canProceed() && v.isSafeToMove()) {
        v.setState(std::make_unique<AcceleratingState>());
    }
}

inline void CruisingState::update(Vehicle& v, float dt) {
    // Nếu cần rẽ, chuyển sang TurningState TRƯỚC KHI kiểm tra phanh
    if (v.shouldTurn()) { 
        v.startTurning(); 
        v.setState(std::make_unique<TurningState>()); 
        return; 
    }
    if (v.shouldBrake()) { 
        v.setState(std::make_unique<BrakingState>()); 
        return; 
    }
    v.moveForward(dt);
}

inline void TurningState::update(Vehicle& v, float dt) {
    v.updateTurning(dt);
    if (v.isTurnComplete()) {
        v.setState(std::make_unique<CruisingState>());
    }
}