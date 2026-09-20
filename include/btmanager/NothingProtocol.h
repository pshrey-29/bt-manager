#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "btmanager/BatteryInfo.h"

namespace btmanager
{

// Proprietary Nothing frame / battery protocol constants.
// Exact meaning of the frame bytes is still unknown; names describe
// position, not purpose.
inline constexpr uint16_t kBatteryCommand = 0xC007;

inline constexpr uint8_t kFrameByte0 = 0x55;
inline constexpr uint8_t kFrameByte1 = 0x60;
inline constexpr uint8_t kFrameByte2 = 0x01;
inline constexpr uint8_t kBatteryResponseByte3 = 0x07;

inline constexpr uint8_t kDeviceLeft = 0x02;
inline constexpr uint8_t kDeviceRight = 0x03;
inline constexpr uint8_t kDeviceCase = 0x04;

inline constexpr uint8_t kPercentageMask = 0x7F;
inline constexpr uint8_t kChargingMask = 0x80;

uint16_t crc16(const uint8_t* data, std::size_t length);

std::vector<uint8_t> buildBatteryCommand(uint8_t operation_id);

bool parseBatteryResponse(
    const uint8_t* data,
    std::size_t length,
    uint8_t expected_operation_id,
    BatteryInfo& battery);

} // namespace btmanager
