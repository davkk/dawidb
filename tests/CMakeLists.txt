enable_testing()

file(
    GLOB SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
)

add_executable(dawidb_test ${SOURCES})

target_link_libraries(
    dawidb_test
    dawidb_lib
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(dawidb_test)
