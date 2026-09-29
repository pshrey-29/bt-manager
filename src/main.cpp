#include "btmanager/BatteryInfo.h"
#include "btmanager/BlueZBattery.h"
#include "btmanager/BluetoothDiscovery.h"
#include "btmanager/NothingEar.h"

#include <iostream>
#include <string>

#include <spdlog/spdlog.h>

void printBatteryInfo(const btmanager::BatteryInfo& battery)
{
    if (battery.percentage)
    {
        std::cout << "  Battery: "
                  << static_cast<int>(*battery.percentage)
                  << "%\n";
    }

    if (battery.left)
    {
        std::cout << "  Left:  "
                  << static_cast<int>(*battery.left)
                  << "% "
                  << (battery.left_charging ? "(charging)" : "")
                  << '\n';
    }

    if (battery.right)
    {
        std::cout << "  Right: "
                  << static_cast<int>(*battery.right)
                  << "% "
                  << (battery.right_charging ? "(charging)" : "")
                  << '\n';
    }

    if (battery.case_battery)
    {
        std::cout << "  Case:  "
                  << static_cast<int>(*battery.case_battery)
                  << "% "
                  << (battery.case_charging ? "(charging)" : "")
                  << '\n';
    }
}

int main(int argc, char* argv[])
{
    bool verbose = false;
    bool debug = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--verbose")
        {
            verbose = true;
        }
        else if (arg == "--debug")
        {
            debug = true;
        }
        else
        {
            spdlog::error("Unknown option: {}", arg);
            return 1;
        }
    }

    if (debug)
    {
        spdlog::set_level(spdlog::level::debug);
    }
    else if (verbose)
    {
        spdlog::set_level(spdlog::level::info);
    }
    else
    {
        spdlog::set_level(spdlog::level::err);
    }

    spdlog::debug(
        "CLI mode: {}",
        debug ? "debug" : verbose ? "verbose" : "normal");

    auto devices = btmanager::getConnectedDevices();

    if (devices.empty())
    {
        std::cout << "No connected Bluetooth devices found.\n";
        return 0;
    }

    for (const auto& device : devices)
    {
        std::cout << device.name << '\n';

        if (device.name == "Nothing Ear (a)")
        {
            btmanager::NothingEar ear(device.address, 15);

            if (!ear.connect())
            {
                spdlog::error("Failed to connect to Nothing Ear (a).");
                return 1;
            }

            auto battery = ear.getBattery();

            if (!battery)
            {
                spdlog::error("Failed to read battery information.");
                return 1;
            }

            printBatteryInfo(*battery);
        }
        else
        {
            auto battery = btmanager::getBatteryLevel(device);

            if (battery)
            {
                printBatteryInfo(*battery);
            }
            else
            {
                std::cout << "  Battery data not available\n";
            }
        }
    }

    return 0;
}
