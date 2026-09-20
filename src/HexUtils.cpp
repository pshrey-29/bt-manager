#include "btmanager/HexUtils.h"

#include <iostream>

#include <iomanip>

namespace btmanager
{

void printHex(const uint8_t* data, std::size_t length, const char* label)
{
    std::cout << label << " (" << length << " bytes): ";
    for (std::size_t i = 0; i < length; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << ' ';
    std::cout << std::dec << '\n';
}

} // namespace btmanager
