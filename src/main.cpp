#include "btmanager/BatteryInfo.h"
#include "btmanager/NothingEar.h"
#include "btmanager/BluetoothDiscovery.h"

#include <iostream>

int main()
{
    auto address = btmanager::findConnectedNothingEar();

    if (!address)
    {
        std::cerr << "No connected Nothing Ear (a) found.\n";
        return 1;
    }

    btmanager::NothingEar ear(*address, 15);

    if (!ear.connect())
    {
        std::cerr << "Failed to connect to Nothing Ear.\n";
        return 1;
    }

    btmanager::BatteryInfo battery;

    if (!ear.getBattery(battery))
    {
        std::cerr << "Failed to read battery.\n";
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
