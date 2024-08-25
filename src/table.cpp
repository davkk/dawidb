#include "table.hpp"

#include <cassert>
#include <print>
#include <vector>

#include "const.hpp"
#include "pager.hpp"
#include "row.hpp"

Table::Table(std::fstream& file) : pager{file} {
    assert(file.is_open());
    num_rows = pager.file_length / ROW_SIZE;
}

Table::~Table() {
    pager.flush(num_rows);
}

std::vector<Row> Table::exec(Statement&& statement) {
    switch (statement.type) {
    case StatementType::SELECT: {
        std::vector<Row> rows;
        auto cursor{Table::begin()};

        while (!cursor.eot) {
            const auto [slot, err]{pager.get(cursor)};
            if (err) {
                Pager::handle_error(*err);
            }
            assert(slot.size() == ROW_SIZE);

            Row row{};
            row.deserialize_from(slot);
            rows.push_back(row);

            this->advance(cursor);
        }

        return rows;
    }
    case StatementType::INSERT: {
        if (num_rows >= MAX_ROWS) {
            std::println(stderr, "[!] max num of rows {} reached", MAX_ROWS);
            return {};
        }

        const auto [slot, err]{pager.get(Table::end())};
        if (err) {
            Pager::handle_error(*err);
        }
        assert(slot.size() == ROW_SIZE);

        auto& row{statement.row_to_insert};
        row.serialize_into(slot);
        num_rows++;

        return {};
    }
    }
}

void Table::advance(Cursor& cursor) const {
    cursor.pos++;
    if (cursor.pos == num_rows) {
        cursor.eot = true;
    }
}
