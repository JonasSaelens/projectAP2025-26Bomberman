cmake_minimum_required(VERSION 3.20)
project(AP_bomberman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(logic)
add_subdirectory(representation)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()