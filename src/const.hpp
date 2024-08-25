#pragma once

#include <cstddef>

#include "row.hpp"

constexpr size_t PAGE_SIZE = 4096;
constexpr size_t MAX_PAGES = 100;

constexpr size_t ROW_SIZE = sizeof(Row);
constexpr size_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr size_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
