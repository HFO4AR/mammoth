# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_PROJECT_NAME mammoth)
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX)

target_sources(app PRIVATE
        src/main.cpp
        src/chassis.cpp
        src/shoot_ring.cpp
        src/aim.cpp
        src/remote.cpp
        motorPack/motor.cpp
        motorPack/Dji_Rm3508.cpp
        motorPack/pid.cpp
        src/load_ring.cpp
        src/can.cpp
)

target_include_directories(app PRIVATE inc src motorPack eigen

)
