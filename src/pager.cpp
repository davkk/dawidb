#include "pager.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <print>

#include "const.hpp"

Pager::Pager(std::fstream &file) : file{file} {
    assert(file.is_open());
    file.seekp(0, std::ios::end);

    auto file_pos{file.tellg()};
    assert(file_pos >= 0);
    file_length = static_cast<size_t>(file_pos);

    file.seekp(0);
    assert(!file.fail());
}

std::optional<PagerError> Pager::write_page(
    const size_t page_num,
    const int64_t write_size
) {
    assert(page_num < MAX_PAGES);
    assert(write_size < PAGE_SIZE);

    const auto &page{pages[page_num]};
    if (page) {
        auto file_pos{static_cast<int64_t>(page_num * PAGE_SIZE)};
        assert(file_pos < MAX_PAGES * PAGE_SIZE);
        file.seekp(file_pos);

        file.write(page.get(), write_size);

        if (file.fail()) {
            return PagerError{
                "failed to write to file",
                PagerErrorCode::WRITE_FAILED
            };
        }
    }

    return std::nullopt;
}

WithError<std::span<char>, PagerError> Pager::find_slot(size_t row_num) {
    assert(row_num < MAX_ROWS);

    auto page_num{row_num / ROWS_PER_PAGE};
    if (page_num >= MAX_PAGES) {
        return {
            {},
            PagerError{
                "page read out of bounds",
                PagerErrorCode::OUT_OF_BOUNDS
            }
        };
    }

    auto &page = pages[page_num];
    if (!page) {
        page = std::make_unique<Page>(PAGE_SIZE);
        auto num_pages{file_length / PAGE_SIZE};
        if (file_length % PAGE_SIZE > 0) {
            num_pages++;
        }

        if (page_num < num_pages) {
            auto file_pos{static_cast<int64_t>(page_num * PAGE_SIZE)};
            file.seekp(file_pos);

            file.read(page.get(), PAGE_SIZE);

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

    assert(page);

    auto offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;
    assert(offset + ROW_SIZE <= PAGE_SIZE);

    return {{&page[offset], ROW_SIZE}, std::nullopt};
}
