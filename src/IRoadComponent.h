#pragma once

class IRoadComponent {
public:
    virtual ~IRoadComponent() = default;
    virtual void displayInfo() const = 0;
};
