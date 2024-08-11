cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_MESSAGE NEVER)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

set(CMAKE_CXX_FLAGS_INIT
    "
    -Wsign-conversion
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wuseless-cast
    -Wdouble-promotion
    -Wformat
    -Wlifetime
    -Wimplicit-fallthrough
    "
)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

project(dawidb VERSION 1.0)

add_executable(dawidb main.cpp)
# target_include_directories(dawidb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_target(
    run
    COMMAND dawidb
    DEPENDS dawidb
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
