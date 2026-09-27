#include "btmanager/NothingEar.h"

#include "btmanager/NothingProtocol.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace btmanager
{

// Sockets may accept fewer bytes than offered per send() call,
// so keep sending from the current offset until all bytes are out.
static bool sendAll(int fd, const uint8_t* data, std::size_t length)
{
    std::size_t offset = 0;

    while (offset < length)
    {
        ssize_t n = send(fd, data + offset, length - offset, 0);

        if (n <= 0)
            return false;

        offset += static_cast<std::size_t>(n);
    }

    return true;
}

NothingEar::NothingEar(std::string mac, uint8_t channel)
    : mac_(std::move(mac))
    , channel_(channel)
    , socket_fd_(-1)
{
}

NothingEar::~NothingEar()
{
    disconnect();
}

bool NothingEar::connect()
{
    // Create an RFCOMM socket.
    socket_fd_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (socket_fd_ < 0)
    {
        std::cerr << "Failed to create socket: "
                  << std::strerror(errno) << '\n';
        return false;
    }

    sockaddr_rc address{};
    address.rc_family = AF_BLUETOOTH;
    address.rc_channel = channel_;

    // Convert MAC address string into Bluetooth address.
    str2ba(mac_.c_str(), &address.rc_bdaddr);

    if (::connect(
            socket_fd_,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0)
    {
        std::cerr << "Failed to connect: "
                  << std::strerror(errno) << '\n';

        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    return true;
}

std::optional<BatteryInfo> NothingEar::getBattery()
{
    if (socket_fd_ < 0)
    {
        std::cerr << "Not connected: call connect() first\n";
        return std::nullopt;
    }

    constexpr uint8_t OPERATION_ID = 5;
    std::vector<uint8_t> command = buildBatteryCommand(OPERATION_ID);

    if (!sendAll(socket_fd_, command.data(), command.size()))
    {
        std::cerr << "Failed to send full command: "
                  << std::strerror(errno) << '\n';
        return std::nullopt;
    }

    // KNOWN LIMITATION: RFCOMM is a byte stream, so a single recv() is
    // not guaranteed to contain a complete protocol response. The
    // current implementation relies on protocol validation and may
    // report failure if a valid response arrives in multiple chunks.
    uint8_t rx[256]{};
    ssize_t received = recv(socket_fd_, rx, sizeof(rx), 0);
    if (received <= 0)
    {
        std::cerr << "Failed to read response: "
                  << std::strerror(errno) << '\n';
        return std::nullopt;
    }

    // Header / op_id / CRC verification and record parsing all live
    // in the protocol layer; just propagate its result.
    BatteryInfo battery;

    if (!parseBatteryResponse(
            rx,
            static_cast<std::size_t>(received),
            OPERATION_ID,
            battery))
    {
        return std::nullopt;
    }

    return battery;
}

void NothingEar::disconnect()
{
    if (socket_fd_ >= 0)
    {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

} // namespace btmanager
