#pragma once

#include <cstddef>
#include <cstdint>

constexpr size_t PAGE_SIZE = 4096;
constexpr size_t MAX_PAGES = 100;

constexpr size_t ID_SIZE = sizeof(uint32_t);
constexpr size_t ID_OFFSET = 0;
constexpr size_t USERNAME_SIZE = 32;
constexpr size_t USERNAME_OFFSET = ID_SIZE;
constexpr size_t EMAIL_SIZE = 255;
constexpr size_t EMAIL_OFFSET = ID_SIZE + USERNAME_SIZE;

constexpr size_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
constexpr size_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr size_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
