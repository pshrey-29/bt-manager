#include "btmanager/BluetoothDiscovery.h"

#include <sdbus-c++/sdbus-c++.h>

#include <map>

#include <spdlog/spdlog.h>

namespace btmanager
{

std::vector<BluetoothDevice> getConnectedDevices()
{
    try
    {
        spdlog::info("Starting Bluetooth discovery");

        auto connection = sdbus::createSystemBusConnection();
        auto proxy = sdbus::createProxy(
            *connection,
            sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{"/"}
        );

        std::map<sdbus::ObjectPath,
                 std::map<sdbus::InterfaceName,
                          std::map<sdbus::PropertyName, sdbus::Variant>>> objects;

        proxy->callMethod("GetManagedObjects")
            .onInterface("org.freedesktop.DBus.ObjectManager")
            .withArguments()
            .storeResultsTo(objects);

        std::vector<BluetoothDevice> devices;

        for (const auto& [objectPath, interfaces] : objects)
        {
            auto deviceIt = interfaces.find(
                sdbus::InterfaceName{"org.bluez.Device1"}
            );

            if (deviceIt == interfaces.end())
                continue;

            const auto& properties = deviceIt->second;

            auto nameIt = properties.find(
                sdbus::PropertyName{"Name"}
            );

            auto addressIt = properties.find(
                sdbus::PropertyName{"Address"}
            );

            auto connectedIt = properties.find(
                sdbus::PropertyName{"Connected"}
            );

            if (nameIt == properties.end() ||
                addressIt == properties.end() ||
                connectedIt == properties.end())
            {
                continue;
            }

            std::string name = nameIt->second.get<std::string>();
            std::string address = addressIt->second.get<std::string>();
            bool connected = connectedIt->second.get<bool>();

            if (!connected)
                continue;

            spdlog::debug(
                "Found device: {} ({})",
                name,
                address);

            devices.push_back({
                name,
                address,
                objectPath
            });
        }

        spdlog::info(
            "Discovery completed: {} connected device(s) found",
            devices.size());

        return devices;
    }
    catch (const sdbus::Error& e)
    {
        spdlog::error(
            "Bluetooth discovery failed: {}: {}",
            e.getName(),
            e.getMessage());
        return {};
    }
}

} // namespace btmanager
