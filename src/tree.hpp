#pragma once

#include <cstdint>
#include <memory>
#include <print>

#include "const.hpp"
#include "cursor.hpp"
#include "row.hpp"

enum class NodeType : bool {
    LEAF,
    INTERNAL,
};

struct Cell {
    uint32_t key;
    Row value;
};

struct Node;

struct Header {
    NodeType type;
    bool is_root;
    size_t num_cells;
    std::shared_ptr<Node> parent;
};

constexpr auto MAX_NODE_CELLS = (PAGE_SIZE - sizeof(Header)) / sizeof(Cell);

struct Node {
    Header header;
    Cell cells[MAX_NODE_CELLS];

    auto show() -> void;
    auto find_cell(uint32_t key) -> uint32_t;
    auto insert(const Cursor& cursor, const Cell& cell) -> void;
};
