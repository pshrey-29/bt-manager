#pragma once

#include "btmanager/BluetoothDevice.h"

#include <vector>

namespace btmanager
{

std::vector<BluetoothDevice> getConnectedDevices();

} // namespace btmanager
