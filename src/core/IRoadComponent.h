#pragma once

#include <memory>
#include <string>
#include <vector>

// Composite Pattern: Road, Intersection (lá) và RoadNetwork (nút ghép) cùng giao diện này.
class IRoadComponent {
public:
    virtual ~IRoadComponent() = default;

    virtual std::string getName() const = 0;
    virtual int getCapacity() const = 0;  // số xe tối đa ước lượng

    // Mặc định (lá): không chứa thành phần con -> addComponent ném std::logic_error.
    virtual void addComponent(std::shared_ptr<IRoadComponent> component);
    virtual std::vector<std::shared_ptr<IRoadComponent>> getConnectedComponents() const;
};
