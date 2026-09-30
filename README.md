# bt-manager

A C++20 Linux utility for inspecting connected Bluetooth devices and retrieving battery information.

The project started with a practical problem: **Nothing Ear (a)** exposes separate left, right, and case battery levels on mobile, while Linux Bluetooth tools generally expose only aggregate battery information.

## Features

* Discover connected Bluetooth devices through **BlueZ D-Bus**
* Read standard battery information through BlueZ `Battery1`
* Retrieve separate left, right, and case battery levels for **Nothing Ear (a)**
* Report charging state for each Nothing Ear (a) component
* Configurable runtime logging with `--verbose` and `--debug`
* Install as a user-local CLI available from anywhere in the terminal

Example:

```text
Nothing Ear (a)
  Left: 95%
  Right: 95%
  Case: 80%

JBL Go 3
  Battery data not available
```

## Nothing Ear (a) Support

Nothing Ear (a) uses a device-specific RFCOMM protocol for its additional battery information.

The protocol used here was previously documented by the Nothing Linux community. This project implements that documented protocol in C++ and validates communication with a physical Nothing Ear (a) device.

The implementation includes:

* RFCOMM communication
* Binary request/response parsing
* Per-request operation IDs
* CRC validation
* Left/right/case battery extraction
* Charging-state extraction

## Dependencies

Ubuntu/Debian:

```bash
sudo apt install \
    build-essential \
    cmake \
    libbluetooth-dev \
    libsystemd-dev \
    libspdlog-dev
```

`sdbus-c++` is fetched and built by CMake.

## Build

```bash
git clone https://github.com/pshrey-29/bt-manager.git
cd bt-manager

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Install locally:

```bash
cmake --install build --prefix ~/.local
```

If `~/.local/bin` is not already in `PATH`:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Then:

```bash
btinfo
```

## Usage

```bash
btinfo
```

Verbose logging:

```bash
btinfo --verbose
```

Debug logging:

```bash
btinfo --debug
```

## Development

Debug build:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

The project separates Bluetooth discovery, standard BlueZ battery access, Nothing Ear communication, and protocol parsing into independent components.
