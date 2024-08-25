#pragma once

#include <cstddef>

#include "cursor.hpp"
#include "pager.hpp"
#include "parser.hpp"

struct Table {
    size_t num_rows;
    Pager pager;

    explicit Table(std::fstream& file);
    ~Table();

    std::vector<Row> exec(Statement&& statement);

    void advance(Cursor& cursor) const;

    [[nodiscard]] auto begin() const {
        return Cursor{0, num_rows == 0};
    }

    [[nodiscard]] auto end() const {
        return Cursor{num_rows, true};
    }
};
