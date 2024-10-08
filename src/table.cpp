#include "table.hpp"

#include <cassert>
#include <print>
#include <vector>

#include "pager.hpp"
#include "row.hpp"
#include "tree.hpp"

Table::Table(std::fstream& file) : pager{file} {
    if (pager.num_pages == 0) {
        auto [root_node, err]{pager.read_page(0)};
        if (err) {
            Pager::handle_error(*err);
        }
        root_node->header.num_cells = 0;
        root_node->header.type = NodeType::LEAF;
        root_node->header.is_root = true;
    }
}

auto Table::show() -> void {
    std::println("Tree:");
    const auto [page, err]{pager.read_page(0)};
    if (not err) {
        page->show();
    }
}

auto Table::exec(Statement&& statement) -> WithError<std::optional<std::vector<Row>>, TableError> {
    switch (statement.type) {
    case StatementType::SELECT: {
        std::vector<Row> rows;
        auto cursor{Table::begin()};

        while (!cursor.eot) {
            auto [row, err]{pager.get_row(cursor)};
            if (err) {
                Pager::handle_error(*err);
            }
            assert(row);

            rows.push_back(*row);
            this->advance_cursor(cursor);
        }

        return {rows, std::nullopt};
    }
    case StatementType::INSERT: {
        auto [node, err]{pager.read_page(root_page_num)};
        if (err) {
            Pager::handle_error(*err);
        }

        assert("unhandled error" && node->header.num_cells < MAX_NODE_CELLS);

        auto& row{statement.row_to_insert};
        auto cursor{Table::find(row.id)};

        if (node->cells[cursor.cell_num].key == row.id) {
            return {
                std::nullopt,
                TableError{
                    TableErrorCode::DUPLICATE_KEY,
                    "duplicate key",
                }
            };
        }

        node->insert(cursor, {.key = row.id, .value = row});
        return {std::vector{row}, std::nullopt};
    }
    }
}

auto Table::begin() -> Cursor {
    auto [root_node, err]{pager.read_page(root_page_num)};
    if (err) {
        Pager::handle_error(*err);
    }

    return Cursor{
        .page_num = root_page_num,
        .cell_num = 0,
        .eot = root_node->header.num_cells == 0,
    };
}

auto Table::find(uint32_t key) -> Cursor {
    auto [root_node, err]{pager.read_page(root_page_num)};
    if (err) {
        Pager::handle_error(*err);
    }

    switch (root_node->header.type) {
    case NodeType::LEAF:
        return Cursor{
            .page_num = root_page_num,
            .cell_num = root_node->find_cell(key),
        };
        break;
    case NodeType::INTERNAL:
        assert("TODO search internal node" && false);
        break;
    }

    return Cursor{
        .page_num = root_page_num,
        .cell_num = 0,
        .eot = root_node->header.num_cells == 0,
    };
}

auto Table::advance_cursor(Cursor& cursor) -> void {
    const auto [node, err]{pager.read_page(cursor.page_num)};
    if (err) {
        Pager::handle_error(*err);
    }

    cursor.cell_num++;
    if (cursor.cell_num >= node->header.num_cells) {
        cursor.eot = true;
    }
}
