cmake_minimum_required(VERSION 3.16)

project(fe3c_tests)
# Set the directory of the code under test
set(FE3C_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../src)
# Allow including headers under test
include_directories(${CMAKE_SOURCE_DIR}/../../include)
include_directories(${CMAKE_SOURCE_DIR}/../../src)

# Fetch and install CppUTest
include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        latest-passing-build
)
FetchContent_MakeAvailable(CppUTest)

include(add_functional_test.cmake)

add_subdirectory(eddsa)
add_subdirectory(field_elements)
add_subdirectory(hash_functions)
add_subdirectory(points)
add_subdirectory(scalars)
