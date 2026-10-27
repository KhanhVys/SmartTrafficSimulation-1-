#pragma once

#include <string>

#include "core/IRoadComponent.h"

// Ngã tư: hình chữ nhật quanh tâm thế giới.
class Intersection : public IRoadComponent {
public:
    Intersection(double halfWidth, double halfHeight);

    std::string getName() const override;
    int getCapacity() const override;

    double getHalfWidth() const;   // nửa kích thước theo trục x
    double getHalfHeight() const;  // nửa kích thước theo trục y

private:
    double halfWidth;
    double halfHeight;
};
