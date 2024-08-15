#include "table.hpp"

#include <cassert>
#include <cstdio>
#include <print>

#include "const.hpp"
#include "pager.hpp"
#include "row.hpp"

constexpr void handle_pager_error(PagerError err) {
    switch (err) {
    case PagerError::OUT_OF_BOUNDS:
        std::println(stderr, "[!] page read out of bounds");
        return;
    case PagerError::READ_FAILED:
        std::println(stderr, "[!] failed to read from file");
        std::abort();
    case PagerError::WRITE_FAILED:
        std::println(stderr, "[!] failed to write to file");
        std::abort();
    }
}

Table::Table(std::string&& file_name) : pager{std::move(file_name)} {
    num_rows = pager.file_length / ROW_SIZE;
}

Table::~Table() {
    auto num_full_pages = num_rows / ROWS_PER_PAGE;
    for (size_t page_num{0}; page_num < num_full_pages; page_num++) {
        auto err{pager.write_page(page_num, PAGE_SIZE)};
        if (err) {
            handle_pager_error(*err);
        }
    }

    size_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        auto err{pager.write_page(
            num_full_pages,
            static_cast<int64_t>(ROW_SIZE * num_additional_rows)
        )};
        if (err) {
            handle_pager_error(*err);
        }
    }

    pager.file.flush();
}

void Table::exec(Statement&& statement) {
    switch (statement.type) {
    case StatementType::SELECT: {
        std::println("=== select statement ===");

        for (size_t idx{0}; idx < num_rows; idx++) {
            Row row{};

            const auto [slot, err]{pager.find_slot(idx)};
            if (err) {
                handle_pager_error(*err);
            }

            row.deserialize_from(slot);
            std::println(
                "{}|{}|{}", row.id, row.username.data(), row.email.data()
            );
        }

        return;
    }
    case StatementType::INSERT: {
        std::println("=== insert statement ===");

        if (num_rows == MAX_ROWS) {
            return;
        }

        const auto [slot, err]{pager.find_slot(num_rows)};
        if (err) {
            handle_pager_error(*err);
        }

        auto& row{statement.row_to_insert};
        std::println(
            "{}|{}|{}", row.id, row.username.data(), row.email.data()
        );
        row.serialize_into(slot);
        num_rows++;

        return;
    }
    }
}
