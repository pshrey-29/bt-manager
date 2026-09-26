#pragma once

#include <string>

namespace btmanager
{

struct BluetoothDevice
{
    std::string name;
    std::string address;
    std::string object_path;
};

} // namespace btmanager
