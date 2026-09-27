#pragma once

#include <cstdint>
#include <optional>

namespace btmanager
{

struct BatteryInfo
{
    std::optional<uint8_t> percentage;

    std::optional<uint8_t> left;
    std::optional<uint8_t> right;
    std::optional<uint8_t> case_battery;

    bool left_charging{false};
    bool right_charging{false};
    bool case_charging{false};
};

} // namespace btmanager
