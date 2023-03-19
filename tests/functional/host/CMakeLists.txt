cmake_minimum_required(VERSION 3.16)

project(fe3c_tests)
# Set the directory of the test code
set(FE3C_UT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../src)
# Set the directory of the code under test
set(FE3C_UUT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../../src)
set(FE3C_UUT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../../include)
# Allow including headers under test
include_directories(${FE3C_UUT_INCLUDE_DIR})
include_directories(${FE3C_UUT_SOURCE_DIR})
# Allow including the common framework header
include_directories("..")

add_compile_definitions(PRIVATE FE3C_FUNCTIONAL_TESTS_HOST)

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
