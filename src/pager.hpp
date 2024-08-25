#pragma once

#include <fstream>
#include <span>

#include "const.hpp"
#include "cursor.hpp"
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

    void flush(size_t num_rows);

    WithError<std::span<char>, PagerError> get(const Cursor &cursor);
    std::optional<PagerError> read(
        const std::unique_ptr<Page> &page,
        std::streamoff file_pos
    );
    std::optional<PagerError> write(
        const std::unique_ptr<Page> &page,
        std::streamoff file_pos,
        int64_t write_size
    );

    static void handle_error(const PagerError &err);
};
