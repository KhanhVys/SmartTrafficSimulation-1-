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

inline void AcceleratingState::update(Vehicle& v, float dt) {
    // Fix vấn đề 7: Tăng tốc cũng phải biết rẽ
    if (v.shouldTurn()) { 
        v.startTurning(); 
        v.setState(std::make_unique<TurningState>()); 
        return; 
    }
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
    
    // Fix vấn đề 2: Không giảm tốc độ về 0 ngay lập tức nếu còn xa vạch
    float dist = v.getDistanceToStopLine();
    if (dist > 20.0f && v.getSpeed() > 20.0f) {
        // Nếu còn xa, chỉ giảm tốc từ từ
        v.setSpeed(v.getSpeed() - (v.getDeceleration() / 2.0f) * dt);
        v.moveForward(dt);
        return;
    }

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
    if (v.shouldTurn()) { 
        v.startTurning(); 
        v.setState(std::make_unique<TurningState>()); 
        return; 
    }
    // Chỉ phanh nếu sắp tới vạch (trong khoảng cách phanh an toàn)
    if (v.shouldBrake() && !v.hasPassedStopLine()) {
        float stoppingDist = (v.getSpeed() * v.getSpeed()) / (2.0f * v.getDeceleration());
        if (v.getDistanceToStopLine() <= stoppingDist + 5.0f) {
            v.setState(std::make_unique<BrakingState>()); 
            return; 
        }
    }
    v.moveForward(dt);
}

inline void TurningState::update(Vehicle& v, float dt) {
    v.updateTurning(dt);
    if (v.isTurnComplete()) {
        v.setState(std::make_unique<CruisingState>());
    }
}