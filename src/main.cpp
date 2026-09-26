#include "btmanager/BatteryInfo.h"
#include "btmanager/BluetoothDiscovery.h"
#include "btmanager/NothingEar.h"

#include <iostream>

int main()
{
    auto devices = btmanager::getConnectedDevices();

    for (const auto& device : devices)
    {
        if (device.name == "Nothing Ear (a)")
        {
            btmanager::NothingEar ear(device.address, 15);

            if (!ear.connect())
            {
                std::cerr << "Failed to connect to Nothing Ear (a).\n";
                return 1;
            }

            btmanager::BatteryInfo battery;

            if (!ear.getBattery(battery))
            {
                std::cerr << "Failed to read battery information.\n";
                return 1;
            }

            std::cout << "Battery:\n";
            std::cout << "  Left:  "
                      << static_cast<int>(battery.left)
                      << "% "
                      << (battery.left_charging ? "(charging)" : "")
                      << '\n';

            std::cout << "  Right: "
                      << static_cast<int>(battery.right)
                      << "% "
                      << (battery.right_charging ? "(charging)" : "")
                      << '\n';

            std::cout << "  Case:  "
                      << static_cast<int>(battery.case_battery)
                      << "% "
                      << (battery.case_charging ? "(charging)" : "")
                      << '\n';

            return 0;
        }
    }

    std::cerr << "No connected Nothing Ear (a) found.\n";
    return 1;
}
