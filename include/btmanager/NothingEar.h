#pragma once

#include <cstdint>
#include <string>

#include "btmanager/BatteryInfo.h"

namespace btmanager
{

// Device-level operations for a Nothing Ear over RFCOMM.
class NothingEar
{
public:
    explicit NothingEar(std::string mac, uint8_t channel);
    ~NothingEar();

    NothingEar(const NothingEar&) = delete;
    NothingEar& operator=(const NothingEar&) = delete;

    bool connect();
    bool getBattery(BatteryInfo& out);
    void disconnect();

private:
    std::string mac_;
    uint8_t channel_;
    int socket_fd_;
};

} // namespace btmanager
