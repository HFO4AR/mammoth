# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_PROJECT_NAME mammoth)
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX)

target_sources(app PRIVATE src/main.cpp

)

target_include_directories(app PRIVATE src

)