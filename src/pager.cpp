#include "pager.hpp"

#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <print>

#include "const.hpp"
#include "cursor.hpp"

void Pager::handle_error(const PagerError &err) {
    std::cerr << std::format("{}\n", err.message);
    switch (err.code) {
    case PagerErrorCode::OUT_OF_BOUNDS:
    case PagerErrorCode::READ_FAILED:
    case PagerErrorCode::WRITE_FAILED:
        std::abort();
    }
}

Pager::Pager(std::fstream &file) : file{file} {
    assert(file.is_open());
    file.seekp(0, std::ios::end);

    auto file_pos{file.tellg()};
    assert(file_pos >= 0);
    file_length = static_cast<size_t>(file_pos);

    file.seekp(0);
    assert(!file.fail());
}

void Pager::flush(size_t num_rows) {
    auto num_full_pages = num_rows / ROWS_PER_PAGE;
    assert(num_full_pages <= MAX_PAGES);

    for (size_t page_num{0}; page_num < num_full_pages; page_num++) {
        if (pages[page_num]) {
            auto err{write(
                pages[page_num],
                static_cast<int64_t>(page_num * PAGE_SIZE),
                PAGE_SIZE
            )};
            if (err) {
                Pager::handle_error(*err);
            }
        }
    }

    size_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0 && pages[num_full_pages]) {
        auto err{write(
            pages[num_full_pages],
            static_cast<int64_t>(num_full_pages * PAGE_SIZE),
            static_cast<int64_t>(ROW_SIZE * num_additional_rows)
        )};
        if (err) {
            Pager::handle_error(*err);
        }
    }

    file.flush();
}

std::optional<PagerError> Pager::write(
    const std::unique_ptr<Page> &page,
    const std::streamoff file_pos,
    const int64_t write_size
) {
    assert(page);
    assert(write_size <= PAGE_SIZE);
    assert(file_pos < MAX_PAGES * PAGE_SIZE);

    file.seekp(file_pos);
    file.write(page.get(), write_size);

    if (file.fail()) {
        return PagerError{
            "failed to write to file",
            PagerErrorCode::WRITE_FAILED
        };
    }

    return std::nullopt;
}

std::optional<PagerError> Pager::read(
    const std::unique_ptr<Page> &page,
    const std::streamoff file_pos
) {
    assert(file_pos < MAX_PAGES * PAGE_SIZE);

    file.seekp(file_pos);
    file.read(page.get(), PAGE_SIZE);

    if (file.eof()) {
        file.clear();
        file.seekp(0);
    }

    if (file.fail()) {
        return PagerError{
            "failed to read from file",
            PagerErrorCode::READ_FAILED
        };
    }

    return std::nullopt;
}

WithError<std::span<char>, PagerError> Pager::get(const Cursor &cursor) {
    auto page_num{cursor.pos / ROWS_PER_PAGE};
    if (page_num >= MAX_PAGES) {
        return {
            {},
            PagerError{
                "page read out of bounds",
                PagerErrorCode::OUT_OF_BOUNDS
            }
        };
    }

    auto &page{pages[page_num]};
    if (!page) {
        page = std::make_unique<Page>(PAGE_SIZE);

        auto num_pages{file_length / PAGE_SIZE};
        if (file_length % PAGE_SIZE > 0) {
            num_pages++;
        }

        if (page_num < num_pages) {
            auto err{
                read(page, static_cast<int64_t>(page_num * PAGE_SIZE))
            };
            if (err) {
                return {{}, err};
            }
        }
    }
    assert(page);

    auto offset = (cursor.pos % ROWS_PER_PAGE) * ROW_SIZE;
    assert(offset + ROW_SIZE <= PAGE_SIZE);

    return {{&page[offset], ROW_SIZE}, std::nullopt};
}
