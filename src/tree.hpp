#pragma once

#include <memory>
#include <print>

constexpr auto PAGE_SIZE = 4096UL;
constexpr auto MAX_PAGES = 100UL;

struct Cell {
    size_t key;
    int value;
};

constexpr auto DEGREE = 3UL;
constexpr auto MAX_CHILDREN = 2UL * DEGREE;
constexpr auto MAX_CELLS = MAX_CHILDREN - 1UL;
constexpr auto MIN_CELLS = DEGREE - 1UL;

struct Node {
    size_t num_cells{0};
    size_t num_children{0};

    std::array<std::shared_ptr<Cell>, MAX_CELLS> cells;
    std::array<std::shared_ptr<Node>, MAX_CHILDREN> children;

    auto show(size_t level) const -> void;
    auto is_leaf() const -> bool;
    auto search(size_t key) -> std::pair<size_t, bool>;
    auto insert_cell(size_t pos, std::shared_ptr<Cell> cell) -> void;
    auto insert_child(size_t pos, std::shared_ptr<Node> node) -> void;
    auto insert(Cell& cell) -> bool;
    auto split() -> std::pair<std::shared_ptr<Cell>, std::shared_ptr<Node>>;
};

struct BTree {
    std::shared_ptr<Node> root{nullptr};

    auto show() const -> void;
    auto find(size_t key) const -> std::optional<int>;
    auto split_root() -> void;
    auto insert(size_t key, int value) -> void;
};