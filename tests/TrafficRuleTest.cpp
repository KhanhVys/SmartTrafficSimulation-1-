#include "Logic/TrafficRule.h"
#include <cassert>
#include <iostream>

int main()
{
    // Den xanh: khong dung, du xe da qua vach hay chua.
    assert(!TrafficRule::shouldStop(LightColor::Green, false));
    assert(!TrafficRule::shouldStop(LightColor::Green, true));

    // Den vang: dung neu chua qua vach.
    assert(TrafficRule::shouldStop(LightColor::Yellow, false));
    assert(!TrafficRule::shouldStop(LightColor::Yellow, true));

    // Den do: dung neu chua qua vach.
    assert(TrafficRule::shouldStop(LightColor::Red, false));
    assert(!TrafficRule::shouldStop(LightColor::Red, true));

    std::cout << "All TrafficRule tests passed!" << std::endl;

    return 0;
}