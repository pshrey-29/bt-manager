#include "btmanager/NothingProtocol.h"

#include "btmanager/HexUtils.h"

#include <spdlog/spdlog.h>

namespace btmanager
{

uint16_t crc16(const uint8_t* data, std::size_t length)
{
    uint16_t crc = 0xFFFF;

    for (std::size_t i = 0; i < length; ++i)
    {
        crc ^= data[i];

        for (int bit = 0; bit < 8; ++bit)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

std::vector<uint8_t> buildBatteryCommand(uint8_t operation_id)
{
    std::vector<uint8_t> cmd = {
        kFrameByte0,
        kFrameByte1,
        kFrameByte2,
        static_cast<uint8_t>(kBatteryCommand & 0xFF),
        static_cast<uint8_t>(kBatteryCommand >> 8),
        0x00,
        0x00,
        operation_id,
    };

    uint16_t crc = crc16(cmd.data(), cmd.size());
    cmd.push_back(static_cast<uint8_t>(crc & 0xFF));
    cmd.push_back(static_cast<uint8_t>(crc >> 8));

    return cmd;
}

bool parseBatteryResponse(
    const uint8_t* data,
    std::size_t length,
    uint8_t expected_operation_id,
    BatteryInfo& battery)
{
    // Minimum response:
    // 4-byte header
    // 3 unknown/protocol bytes
    // 1-byte operation ID
    // 1-byte component count
    // 2-byte record per component [component_id] [battery byte]
    // 2-byte CRC
    if (length < 11)
    {
        spdlog::error(
            "Invalid response size: {} bytes (minimum 11)", length);
        return false;
    }

    // Response header
    if (!(data[0] == kFrameByte0 &&
          data[1] == kFrameByte1 &&
          data[2] == kFrameByte2 &&
          data[3] == kBatteryResponseByte3))
    {
        spdlog::error(
            "Invalid response header: {}",
            toHexString(data, 4));
        return false;
    }

    // Operation ID
    if (data[7] != expected_operation_id)
    {
        spdlog::error(
            "Operation ID mismatch: expected {}, received {}",
            expected_operation_id,
            data[7]);
        return false;
    }

    // CRC
    uint16_t calculated_crc =
        crc16(data, length - 2);

    uint16_t received_crc =
        static_cast<uint16_t>(data[length - 2]) |
        (static_cast<uint16_t>(data[length - 1]) << 8);

    if (calculated_crc != received_crc)
    {
        spdlog::error(
            "CRC mismatch: calculated {:04x}, received {:04x}",
            calculated_crc,
            received_crc);
        return false;
    }

    // Number of battery records
    uint8_t component_count = data[8];

    spdlog::debug(
        "Response operation ID: {}, component count: {}",
        data[7],
        component_count);

    // Records start at byte 9.
    // Each record is:
    //
    // [device ID] [battery byte]
    //
    std::size_t records_end = 9 + (component_count * 2);

    // Leave the final 2 bytes for CRC.
    if (records_end + 2 > length)
    {
        spdlog::error(
            "Invalid component count: {} (needs {} bytes, have {})",
            component_count,
            records_end + 2,
            length);
        return false;
    }

    for (uint8_t i = 0; i < component_count; ++i)
    {
        std::size_t index = 9 + (i * 2);

        uint8_t device_id = data[index];
        uint8_t battery_byte = data[index + 1];

        uint8_t percentage =
            battery_byte & kPercentageMask;

        bool charging =
            (battery_byte & kChargingMask) != 0;

        spdlog::debug(
            "Component {:#04x}: {}%{}",
            device_id,
            percentage,
            charging ? " (charging)" : "");

        switch (device_id)
        {
            case kDeviceLeft:
                battery.left = percentage;
                battery.left_charging = charging;
                break;

            case kDeviceRight:
                battery.right = percentage;
                battery.right_charging = charging;
                break;

            case kDeviceCase:
                battery.case_battery = percentage;
                battery.case_charging = charging;
                break;

            default:
                spdlog::debug(
                    "Unknown battery device ID: {:#04x}, ignoring",
                    device_id);
                break;
        }
    }

    return true;
}

} // namespace btmanager
