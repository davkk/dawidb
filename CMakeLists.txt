cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_INIT
    "-stdlib=libc++ -fno-exceptions -Wall -Wshadow -pedantic -Wsign-conversion"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_INSTALL_MESSAGE NEVER)
# set(CMAKE_VERBOSE_MAKEFILE OFF)
# set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
# set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

project(dawidb VERSION 0.1)

add_executable(dawidb main.cpp)

include(CTest)

add_subdirectory(external)
add_subdirectory(tests)
add_subdirectory(src)

target_link_libraries(dawidb src)

add_custom_target(
    run
    COMMAND dawidb
    DEPENDS dawidb
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_compile_features(dawidb PRIVATE cxx_std_23)
