cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_MESSAGE NEVER)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

project(dawidb VERSION 1.0)

add_executable(dawidb main.cpp)
# target_include_directories(dawidb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_target(
    run
    COMMAND dawidb
    DEPENDS dawidb
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
