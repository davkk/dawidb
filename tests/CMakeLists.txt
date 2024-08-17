enable_testing()

file(
    GLOB SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
)

add_executable(dawidb_test ${SOURCES})

target_link_libraries(dawidb_test GTest::gtest_main)

target_include_directories(dawidb_test PRIVATE "${PROJECT_SOURCE_DIR}/src")

include(GoogleTest)
gtest_discover_tests(dawidb_test)
