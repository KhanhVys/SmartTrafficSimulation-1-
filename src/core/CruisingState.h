#pragma once

#include "core/IVehicleState.h"

class CruisingState : public IVehicleState {
public:
    const char* getName() const override;
    std::unique_ptr<IVehicleState> update(Vehicle& vehicle, double dt, const WorldView& view) override;
};
