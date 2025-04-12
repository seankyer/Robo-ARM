# RoboARM Project
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app)

target_sources(app PRIVATE
        src/main.c
        src/pathfinding/spaces.c
        src/pathfinding/pathfinding.c
        src/pathfinding/graph/graph.c
        src/utils/utils.c
)

target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/utils
        ${CMAKE_SOURCE_DIR}/src/pathfinding
        ${CMAKE_SOURCE_DIR}/src/pathfinding/graph
)
