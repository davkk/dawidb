#pragma once

#include <cstdint>

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;

constexpr uint32_t SIZE_ID = sizeof(uint32_t);
constexpr uint32_t OFFSET_ID = 0;
constexpr uint32_t SIZE_USERNAME = 32;
constexpr uint32_t OFFSET_USERNAME = SIZE_ID;
constexpr uint32_t SIZE_EMAIL = 255;
constexpr uint32_t OFFSET_EMAIL = SIZE_ID + SIZE_USERNAME;

constexpr uint32_t ROW_SIZE = SIZE_ID + SIZE_USERNAME + SIZE_EMAIL;
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
