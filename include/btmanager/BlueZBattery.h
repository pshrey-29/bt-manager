#pragma once

#include "btmanager/BluetoothDevice.h"

#include <cstdint>
#include <optional>

namespace btmanager
{

std::optional<uint8_t> getBatteryLevel(
    const BluetoothDevice& device);

} // namespace btmanager
