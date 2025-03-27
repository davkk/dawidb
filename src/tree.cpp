#include "tree.hpp"

#include <cassert>
#include <ranges>
#include <utility>

auto Node::is_leaf() const -> bool {
    return num_children == 0;
}

auto Node::search(size_t key) -> std::pair<size_t, bool> {
    auto low{0UL};
    auto high{num_cells};

    while (low < high) {
        auto mid{(low + high) / 2UL};
        auto cell{cells[mid]};

        if (cell->key == key) {
            return {mid, true};
        }

        if (cell->key < key) {
            low = mid + 1;
        } else if (cell->key > key) {
            high = mid;
        }
    }

    return std::make_pair(low, false);
}

auto Node::insert_cell(size_t pos, std::shared_ptr<Cell> cell) -> void {
    assert(num_cells < MAX_CELLS);

    if (pos < num_cells) {
        for (auto idx{num_cells}; idx > pos; --idx) {
            cells[idx] = cells[idx - 1];
        }
    }

    cells[pos] = std::move(cell);
    ++num_cells;
}

auto Node::insert_child(size_t pos, std::shared_ptr<Node> node) -> void {
    assert(num_children < MAX_CHILDREN);

    if (pos < num_children) {
        for (auto idx{num_children}; idx > pos; --idx) {
            children[idx] = children[idx - 1];
        }
    }

    children[pos] = std::move(node);
    ++num_children;
}

auto Node::insert(Cell& cell) -> bool {
    auto [pos, found]{search(cell.key)};
    auto new_cell{std::make_shared<Cell>(cell)};

    if (found) {
        cells[pos] = new_cell;
        return false;
    }

    if (is_leaf()) {
        insert_cell(pos, new_cell);
        return true;
    }

    if (children[pos]->num_cells >= MAX_CELLS) {
        auto [mid_cell, new_node]{children[pos]->split()};

        insert_cell(pos, mid_cell);
        insert_child(pos + 1, new_node);

        if (cell.key > cells[pos]->key) {
            pos++;
        } else if (cell.key == cells[pos]->key) {
            cells[pos] = new_cell;
            return true;
        }
    }

    return children[pos]->insert(cell);
}

auto Node::split() -> std::pair<std::shared_ptr<Cell>, std::shared_ptr<Node>> {
    assert(num_cells > MIN_CELLS);
    auto mid{MIN_CELLS};
    auto mid_cell{cells[mid]};

    auto new_node{std::make_shared<Node>()};
    std::ranges::copy(cells | std::views::drop(mid + 1), new_node->cells.begin());
    new_node->num_cells = num_cells - mid - 1;
    num_cells = mid;

    if (!is_leaf()) {
        std::ranges::copy(children | std::views::drop(mid + 1), new_node->children.begin());
        new_node->num_children = num_children - mid - 1;
        num_children = mid + 1;
    }

    return std::make_pair(mid_cell, new_node);
}

auto Node::show(size_t level) const -> void {
    std::string indent(level * 4UL, ' ');

    std::print("{}-", indent);
    for (size_t i = 0; i < num_cells; ++i) {
        std::print("[{}: {}]", cells[i]->key, cells[i]->value);
    }
    std::println();

    if (!is_leaf()) {
        for (size_t i = 0; i < num_children; ++i) {
            children[i]->show(level + 1);
        }
    }
}

auto BTree::show() const -> void {
    root->show(0);
}

auto BTree::find(size_t key) const -> std::optional<int> {
    auto curr{root};
    while (curr) {
        const auto& [pos, found]{curr->search(key)};
        if (found) {
            return curr->cells[pos]->value;
        }
        curr = curr->children[pos];
    }
    return std::nullopt;
}

auto BTree::split_root() -> void {
    auto new_root{std::make_shared<Node>()};
    auto [cell, node]{root->split()};

    new_root->insert_cell(0, cell);
    new_root->insert_child(0, root);
    new_root->insert_child(1, node);

    root = new_root;
}

auto BTree::insert(size_t key, int value) -> void {
    auto cell{Cell{.key = key, .value = value}};

    if (!root) {
        root = std::make_shared<Node>();
    }

    if (root->num_cells >= MAX_CELLS) {
        split_root();
    }

    root->insert(cell);
}