#include "btmanager/HexUtils.h"

#include <iomanip>
#include <sstream>

namespace btmanager
{

std::string toHexString(const uint8_t* data, std::size_t length)
{
    std::ostringstream out;
    out << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < length; ++i)
    {
        if (i > 0)
            out << ' ';

        out << std::setw(2) << static_cast<int>(data[i]);
    }

    return out.str();
}

} // namespace btmanager
