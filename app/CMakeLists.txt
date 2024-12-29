# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(led_strip)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

# add_subdirectory(src/bt)
add_subdirectory(src/led_player)
add_subdirectory(src/touch_controller)
add_subdirectory(src/factory_settings)
add_subdirectory(src/context_storage)
add_subdirectory_ifdef(CONFIG_PM src/low_power)

target_include_directories(app PRIVATE
    src)