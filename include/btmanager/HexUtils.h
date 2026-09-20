#pragma once

#include <cstddef>
#include <cstdint>

namespace btmanager
{

// General-purpose debug helper: hex-dump a byte buffer.
void printHex(const uint8_t* data, std::size_t length, const char* label);

} // namespace btmanager
