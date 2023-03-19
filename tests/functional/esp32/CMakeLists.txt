# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(FE3C_UUT_COMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../../src)
set(FE3C_UUT_INCLUDE_DIR       ${CMAKE_SOURCE_DIR}/../../../include)
set(FE3C_UT_COMMON_SOURCE_DIR  ${CMAKE_SOURCE_DIR}/../src)

set(EXTRA_COMPONENT_DIRS       ${CMAKE_SOURCE_DIR}/../../../esp32)
set(TEST_COMPONENTS "cementite" CACHE STRING "List of components to test")

# Allow including the common framework header
include_directories(..)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(functional)
