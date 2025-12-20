# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(BOARD_ROOT "${CMAKE_SOURCE_DIR}")
set(BOARD "bee_stm32f4")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_PROJECT_NAME mammoth)
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX)

target_sources(app PRIVATE
#        src/*.cpp
        src/main.cpp
        src/chassis.cpp
        src/shoot_ring.cpp
        src/aim.cpp
        src/remote.cpp
        motorPack/motor.cpp
        motorPack/can_motor.cpp
        motorPack/dji_m3508.cpp
        motorPack/pid.cpp
        motorPack/deeprobotics_j60.cpp
        src/load_ring.cpp
        src/can.cpp
        src/dji_dbus.cpp
        src/motor_music.cpp
        src/upper_computer_communication.cpp
        src/serial.cpp
        motorPack/pwm_motor.cpp
        motorPack/pwm_servo.cpp

)

target_include_directories(app PRIVATE inc src motorPack eigen

)
