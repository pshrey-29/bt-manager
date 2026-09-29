#include "btmanager/NothingEar.h"

#include "btmanager/HexUtils.h"
#include "btmanager/NothingProtocol.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

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
    spdlog::info("Attempting RFCOMM connection");
    spdlog::debug(
        "Target: {} on RFCOMM channel {}",
        mac_,
        channel_);

    // Create an RFCOMM socket.
    socket_fd_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (socket_fd_ < 0)
    {
        spdlog::error(
            "Failed to create RFCOMM socket: {}",
            std::strerror(errno));
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
        spdlog::error(
            "RFCOMM connection failed: {}",
            std::strerror(errno));

        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    spdlog::info("RFCOMM connection to {} successful", mac_);

    return true;
}

std::optional<BatteryInfo> NothingEar::getBattery()
{
    if (socket_fd_ < 0)
    {
        spdlog::error("Not connected: call connect() first");
        return std::nullopt;
    }

    constexpr uint8_t OPERATION_ID = 5;
    std::vector<uint8_t> command = buildBatteryCommand(OPERATION_ID);

    if (!sendAll(socket_fd_, command.data(), command.size()))
    {
        spdlog::error(
            "Failed to send battery command: {}",
            std::strerror(errno));
        return std::nullopt;
    }

    spdlog::debug(
        "TX ({} bytes): {}",
        command.size(),
        toHexString(command.data(), command.size()));

    // KNOWN LIMITATION: RFCOMM is a byte stream, so a single recv() is
    // not guaranteed to contain a complete protocol response. The
    // current implementation relies on protocol validation and may
    // report failure if a valid response arrives in multiple chunks.
    uint8_t rx[256]{};
    ssize_t received = recv(socket_fd_, rx, sizeof(rx), 0);
    if (received <= 0)
    {
        spdlog::error(
            "Failed to read battery response: {}",
            std::strerror(errno));
        return std::nullopt;
    }

    spdlog::debug(
        "RX ({} bytes): {}",
        received,
        toHexString(
            rx,
            static_cast<std::size_t>(received)));

    // Header / op_id / CRC verification and record parsing all live
    // in the protocol layer, which logs its own rejections;
    // just propagate the result.
    BatteryInfo battery;

    if (!parseBatteryResponse(
            rx,
            static_cast<std::size_t>(received),
            OPERATION_ID,
            battery))
    {
        return std::nullopt;
    }

    spdlog::info("Battery request completed successfully");

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
