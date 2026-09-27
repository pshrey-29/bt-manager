#include "btmanager/BlueZBattery.h"

#include <sdbus-c++/sdbus-c++.h>

namespace btmanager
{

std::optional<uint8_t> getBatteryLevel(
    const BluetoothDevice& device)
{
    try
    {
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

        return value.get<uint8_t>();
    }
    catch (const sdbus::Error&)
    {
        // Interface/property absent or call failed: this device exposes no usable Battery1 data.
        return std::nullopt;
    }
}

} // namespace btmanager
