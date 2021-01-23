# For Brew installs
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

cmake_minimum_required(VERSION 3.7)
project(CHIP_8_Emulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/chip8.cpp)
add_executable(chip8 ${SOURCE_FILES})

find_package(SDL2 REQUIRED)

# Link Brew Headers
target_link_libraries(chip8 SDL2)