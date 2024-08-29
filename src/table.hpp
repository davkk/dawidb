#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include "cursor.hpp"
#include "pager.hpp"
#include "parser.hpp"

struct Table {
    Pager pager;
    size_t root_page_num{0};

    explicit Table(std::fstream &file);

    Table(const Table &) = delete;
    Table(Table &&) = delete;
    Table &operator=(const Table &) = delete;
    Table &operator=(Table &&) = delete;

    void show();
    std::vector<Row> exec(Statement &&statement);

    void advance_cursor(Cursor &cursor);

    [[nodiscard]] auto begin() {
        auto [root_node, err]{pager.read(root_page_num)};
        if (err) {
            Pager::handle_error(*err);
        }

        return Cursor{
            .page_num = root_page_num,
            .cell_num = 0,
            .eot = root_node->header.num_cells == 0,
        };
    }

    [[nodiscard]] auto end() {
        auto [root_node, err]{pager.read(root_page_num)};
        if (err) {
            Pager::handle_error(*err);
        }

        return Cursor{
            .page_num = root_page_num,
            .cell_num = root_node->header.num_cells,
            .eot = true,
        };
    }
};
