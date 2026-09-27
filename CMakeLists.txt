cmake_minimum_required(VERSION 3.20)

project(bt-manager
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    sdbus-cpp
    GIT_REPOSITORY https://github.com/Kistler-Group/sdbus-cpp.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(sdbus-cpp)

add_executable(btinfo
    src/main.cpp
    src/HexUtils.cpp
    src/NothingEar.cpp
    src/NothingProtocol.cpp
    src/BluetoothDiscovery.cpp
    src/BlueZBattery.cpp
)

target_include_directories(btinfo
    PRIVATE
        include
)

target_link_libraries(btinfo
    PRIVATE
        SDBusCpp::sdbus-c++
        bluetooth
)
