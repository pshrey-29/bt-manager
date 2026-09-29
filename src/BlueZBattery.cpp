#include "btmanager/BlueZBattery.h"

#include <sdbus-c++/sdbus-c++.h>

#include <spdlog/spdlog.h>

namespace btmanager
{

std::optional<BatteryInfo> getBatteryLevel(
    const BluetoothDevice& device)
{
    try
    {
        spdlog::info("Querying Battery1 for {}", device.name);

        auto connection = sdbus::createSystemBusConnection();

        auto proxy = sdbus::createProxy(
            *connection,
            sdbus::ServiceName{"org.bluez"},
            sdbus::ObjectPath{device.object_path}
        );

        // org.freedesktop.DBus.Properties.Get returns the value wrapped in a variant
        // Battery1.Percentage is a D-Bus byte (y), which maps to uint8_t.
        sdbus::Variant value;

        proxy->callMethod("Get")
            .onInterface("org.freedesktop.DBus.Properties")
            .withArguments(
                sdbus::InterfaceName{"org.bluez.Battery1"},
                sdbus::PropertyName{"Percentage"})
            .storeResultsTo(value);

        BatteryInfo battery;
        battery.percentage = value.get<uint8_t>();

        if (*battery.percentage > 100)
        {
            spdlog::error(
                "Invalid Battery1 percentage for {}: {}",
                device.name,
                *battery.percentage);
            return std::nullopt;
        }

        spdlog::debug(
            "Battery1 for {}: {}%",
            device.name,
            *battery.percentage);

        return battery;
    }
    catch (const sdbus::Error& e)
    {
        // Interface/property absent or call failed: this device exposes no usable Battery1 data.
        const std::string error_name = e.getName();

        if (error_name == "org.freedesktop.DBus.Error.UnknownMethod" ||
            error_name == "org.freedesktop.DBus.Error.UnknownInterface" ||
            error_name == "org.freedesktop.DBus.Error.InvalidArgs")
        {
            spdlog::debug("Battery1 not available for {}", device.name);
        }
        else
        {
            spdlog::error(
                "Battery1 query failed for {}: {}: {}",
                device.name,
                error_name,
                e.getMessage());
        }

        return std::nullopt;
    }
}

} // namespace btmanager
