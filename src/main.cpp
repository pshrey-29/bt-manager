#include "btmanager/BlueZBattery.h"
#include "btmanager/BluetoothDiscovery.h"

#include <iostream>

int main()
{
    auto devices = btmanager::getConnectedDevices();

    for (const auto& device : devices)
    {
        auto battery = btmanager::getBatteryLevel(device);

        if (battery)
        {
            std::cout << device.name
                      << ": "
                      << static_cast<int>(*battery)
                      << "%\n";
        }
        else
        {
            std::cout << device.name
                      << ": Battery1 unavailable\n";
        }
    }

    return 0;
}
