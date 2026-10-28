#include "Logic/TrafficRule.h"
#include <cassert>
#include <iostream>

int main()
{
    // Đèn xanh: xe luôn được đi tiếp.
    assert(!TrafficRule::shouldStop(LightColor::Green, false));
    assert(!TrafficRule::shouldStop(LightColor::Green, true));

    // Đèn vàng: chưa qua vạch thì dừng, qua vạch thì đi tiếp.
    assert(TrafficRule::shouldStop(LightColor::Yellow, false));
    assert(!TrafficRule::shouldStop(LightColor::Yellow, true));

    // Đèn đỏ: chưa qua vạch thì dừng, qua vạch thì đi tiếp.
    assert(TrafficRule::shouldStop(LightColor::Red, false));
    assert(!TrafficRule::shouldStop(LightColor::Red, true));

    std::cout << "All TrafficRule tests passed!" << std::endl;
    return 0;
}