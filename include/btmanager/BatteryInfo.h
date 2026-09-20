#pragma once

#include <cstdint>

namespace btmanager
{

struct BatteryInfo
{
    uint8_t left{};
    uint8_t right{};
    uint8_t case_battery{};

    bool left_charging{false};
    bool right_charging{false};
    bool case_charging{false};
};

} // namespace btmanager
