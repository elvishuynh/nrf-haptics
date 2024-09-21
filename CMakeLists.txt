#/Users/elvishuynh/Icarus/ncs-workspace/apollos-lyre/blinky/CMakeLists.txt

cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../nrf-drv2605-driver)
message(STATUS "ZEPHYR_EXTRA_MODULES: ${ZEPHYR_EXTRA_MODULES}")
foreach(module ${ZEPHYR_EXTRA_MODULES})
  message(STATUS "Processing extra module: ${module}")
endforeach()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
# NORDIC SDK APP END
