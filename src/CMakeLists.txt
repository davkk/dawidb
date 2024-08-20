file(
    GLOB SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
)

add_library(dawidb_lib STATIC ${SOURCES})
target_include_directories(dawidb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
