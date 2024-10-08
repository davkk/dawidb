#include "pager.hpp"

#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <print>

#include "cursor.hpp"
#include "row.hpp"

Pager::Pager(std::fstream& file) : file{file} {
    assert(file.is_open());
    file.seekp(0, std::ios::end);

    auto file_pos{file.tellg()};
    assert(file_pos >= 0);
    file_length = static_cast<size_t>(file_pos);

    file.seekp(0);
    assert(!file.fail());

    num_pages = file_length / PAGE_SIZE;
    assert("file contains full pages" && file_length % PAGE_SIZE == 0);
}

Pager::~Pager() {
    for (auto page_num{0UL}; page_num < num_pages; page_num++) {
        auto err{write_page(page_num, static_cast<int64_t>(page_num * PAGE_SIZE))};
        if (err) {
            Pager::handle_error(*err);
        }
    }

    file.flush();
}

auto Pager::handle_error(const PagerError& err) -> void {
    std::cerr << std::format("{}\n", err.message);
    switch (err.code) {
    case PagerErrorCode::OUT_OF_BOUNDS:
    case PagerErrorCode::READ_FAILED:
    case PagerErrorCode::WRITE_FAILED:
    case PagerErrorCode::PAGE_NOT_EXIST:
        std::abort();
    }
}

auto Pager::write_page(const size_t page_num, const std::streamoff file_pos) -> std::optional<PagerError> {
    assert(page_num <= MAX_PAGES);
    assert(file_pos < MAX_PAGES * PAGE_SIZE);

    if (const auto& page{pages[page_num]}) {
        file.seekp(file_pos);
        file.write(reinterpret_cast<char*>(page.get()), PAGE_SIZE);

        if (file.fail()) {
            return PagerError{
                PagerErrorCode::WRITE_FAILED,
                "failed to write to file",
            };
        }

        return std::nullopt;
    }

    return PagerError{
        PagerErrorCode::PAGE_NOT_EXIST,
        "page doesn't exist",
    };
}

auto Pager::read_page(const size_t page_num) -> WithError<Page*, PagerError> {
    auto& page{pages[page_num]};

    if (!page) {
        page = std::make_unique<Page>();

        if (page_num >= num_pages) {
            num_pages++;
        }

        if (page_num < num_pages) {
            auto file_pos{static_cast<int64_t>(page_num * PAGE_SIZE)};
            assert(file_pos < MAX_PAGES * PAGE_SIZE);

            file.seekp(file_pos);
            file.read(reinterpret_cast<char*>(page.get()), PAGE_SIZE);

            if (file.eof()) {
                file.clear();
                file.seekp(0);
            }

            if (file.fail()) {
                return {
                    nullptr,
                    PagerError{
                        PagerErrorCode::READ_FAILED,
                        "failed to read from file",
                    },
                };
            }
        }
    }

    return {page.get(), std::nullopt};
}

auto Pager::get_row(const Cursor& cursor) -> WithError<Row*, PagerError> {
    if (cursor.page_num >= MAX_PAGES) {
        return {
            nullptr,
            PagerError{
                PagerErrorCode::OUT_OF_BOUNDS,
                "page read out of bounds",
            },
        };
    }

    auto [page, err]{read_page(cursor.page_num)};
    if (err) {
        Pager::handle_error(*err);
    }
    assert(page);

    return {&page->cells[cursor.cell_num].value, std::nullopt};
}
