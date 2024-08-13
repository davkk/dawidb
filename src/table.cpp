#include "table.hpp"

#include <cassert>
#include <format>
#include <iostream>
#include <print>

#include "const.hpp"
#include "row.hpp"

void Table::exec(Statement&& statement) {
    switch (statement.type) {
    case StatementType::SELECT: {
        std::println("select statement");

        Row row{};

        for (size_t idx{0}; idx < num_rows; idx++) {
            const auto [slot, err]{find_slot(idx)};
            if (not err) {
                row.deserialize_from(slot);
                std::cout << std::format(
                    "{}|{}|{}\n", row.id, row.username.data(), row.email.data()
                );
            }
        }

        return;
    }
    case StatementType::INSERT: {
        std::cout << "\ninsert statement\n";

        if (num_rows == MAX_ROWS) {
            return;
        }

        const auto [slot, err]{find_slot(num_rows)};
        if (not err) {
            auto& row{statement.row_to_insert};
            std::cout << std::format(
                "{}|{}|{}\n", row.id, row.username.data(), row.email.data()
            );
            row.serialize_into(slot);
            num_rows++;
        }

        return;
    }
    }
}

WithError<std::span<char>, PagerError> Table::find_slot(size_t row_num) {
    auto page_num{row_num / ROWS_PER_PAGE};

    if (page_num >= MAX_PAGES) {
        return {{}, PagerError::OUT_OF_RANGE};
    }

    auto offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;

    assert(page_num < pages.size());
    assert(offset < pages[page_num].size());

    return {{&pages[page_num][offset], ROW_SIZE}, std::nullopt};
}
