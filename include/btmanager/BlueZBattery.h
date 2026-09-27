#pragma once

#include "btmanager/BatteryInfo.h"
#include "btmanager/BluetoothDevice.h"

#include <optional>

namespace btmanager
{

std::optional<BatteryInfo> getBatteryLevel(
    const BluetoothDevice& device);

} // namespace btmanager
