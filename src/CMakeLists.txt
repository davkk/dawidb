add_library(src STATIC table.cpp pager.cpp row.cpp parser.cpp)
target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
