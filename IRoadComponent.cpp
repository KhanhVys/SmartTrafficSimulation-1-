#include "core/IRoadComponent.h"

#include <stdexcept>

void IRoadComponent::addComponent(std::shared_ptr<IRoadComponent>) {
    throw std::logic_error("Thanh phan la khong chua duoc thanh phan con: " + getName());
}

std::vector<std::shared_ptr<IRoadComponent>> IRoadComponent::getConnectedComponents() const {
    return {};
}
