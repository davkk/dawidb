#include "pager.hpp"

#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <print>

#include "cursor.hpp"
#include "row.hpp"

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

    num_pages = file_length / PAGE_SIZE;
    assert(file_length % PAGE_SIZE == 0 && "file contains full pages");
}

Pager::~Pager() {
    for (size_t page_num{0}; page_num < num_pages; page_num++) {
        auto err{write(page_num, static_cast<int64_t>(page_num * PAGE_SIZE))};
        if (err) {
            Pager::handle_error(*err);
        }
    }

    file.flush();
}

std::optional<PagerError> Pager::write(
    const size_t page_num,
    const std::streamoff file_pos
) {
    assert(page_num <= MAX_PAGES);
    assert(file_pos < MAX_PAGES * PAGE_SIZE);

    if (const auto &page{pages[page_num]}) {
        file.seekp(file_pos);
        file.write(reinterpret_cast<char *>(page.get()), PAGE_SIZE);

        if (file.fail()) {
            return PagerError{
                "failed to write to file",
                PagerErrorCode::WRITE_FAILED
            };
        }
    }

    return std::nullopt;
}

WithError<const std::unique_ptr<Page> &, PagerError> Pager::read(
    const size_t page_num
) {
    auto &page{pages[page_num]};

    if (!page) {
        page = std::make_unique<Page>();

        if (page_num >= num_pages) {
            num_pages++;
        }

        if (page_num < num_pages) {
            auto file_pos{static_cast<int64_t>(page_num * PAGE_SIZE)};
            assert(file_pos < MAX_PAGES * PAGE_SIZE);

            file.seekp(file_pos);
            file.read(reinterpret_cast<char *>(page.get()), PAGE_SIZE);

            if (file.eof()) {
                file.clear();
                file.seekp(0);
            }

            if (file.fail()) {
                return {
                    {},
                    PagerError{
                        "failed to read from file",
                        PagerErrorCode::READ_FAILED
                    }
                };
            }
        }
    }

    return {page, std::nullopt};
}

WithError<std::span<char>, PagerError> Pager::get_row(const Cursor &cursor) {
    if (cursor.page_num >= MAX_PAGES) {
        return {
            {},
            PagerError{
                "page read out of bounds",
                PagerErrorCode::OUT_OF_BOUNDS
            }
        };
    }

    auto [page, err]{read(cursor.page_num)};
    if (err) {
        Pager::handle_error(*err);
    }
    assert(page);

    auto *data{reinterpret_cast<char *>(&page->cells[cursor.cell_num].value)};
    return {{data, ROW_SIZE}, std::nullopt};
}
