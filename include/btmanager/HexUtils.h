#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace btmanager
{

// Format a byte buffer as space-separated hex ("55 60 01").
// Used for debug logging of raw protocol frames.
std::string toHexString(const uint8_t* data, std::size_t length);

} // namespace btmanager
