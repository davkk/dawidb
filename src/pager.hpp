#pragma once

#include <fstream>
#include <span>

#include "const.hpp"
#include "with_error.hpp"

using Page = char[];

enum PagerErrorCode {
    OUT_OF_BOUNDS = 1,
    READ_FAILED,
    WRITE_FAILED,
};

struct PagerError {
    std::string_view message;
    PagerErrorCode code;
};

struct Pager {
    std::fstream &file;
    size_t file_length{0};
    std::array<std::unique_ptr<Page>, MAX_PAGES> pages{};

    explicit Pager(std::fstream &file);
    Pager(const Pager &) = delete;
    Pager(Pager &&) = delete;
    Pager &operator=(const Pager &) = delete;
    Pager &operator=(Pager &&) = delete;

    WithError<std::span<char>, PagerError> find_slot(size_t row_num);

    std::optional<PagerError> write_page(size_t page_num, int64_t write_size);
};
