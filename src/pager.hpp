#pragma once

#include <fstream>
#include <span>

#include "const.hpp"
#include "cursor.hpp"
#include "tree.hpp"
#include "with_error.hpp"

using Page = Node;

enum PagerErrorCode {
    OUT_OF_BOUNDS = 1,
    READ_FAILED,
    WRITE_FAILED,
};

struct PagerError {
    PagerErrorCode code;
    std::string_view message;
};

struct Pager {
    std::fstream& file;
    size_t file_length{0};

    std::array<std::unique_ptr<Page>, MAX_PAGES> pages{};
    size_t num_pages{0};

    explicit Pager(std::fstream& file);
    ~Pager();

    Pager(const Pager&) = delete;
    Pager(Pager&&) = delete;
    Pager& operator=(const Pager&) = delete;
    Pager& operator=(Pager&&) = delete;

    auto flush() -> void;

    auto get_row(const Cursor& cursor) -> WithError<std::span<char>, PagerError>;
    auto read(size_t page_num) -> WithError<Page*, PagerError>;
    auto write(size_t page_num, std::streamoff file_pos) -> std::optional<PagerError>;

    static void handle_error(const PagerError& err);
};
