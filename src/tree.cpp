#include "tree.hpp"

#include <cassert>
#include <utility>

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

    return {low, false};
}

auto Node::insert_cell(size_t pos, std::shared_ptr<Cell> cell) -> void {
    assert(pos < MAX_CELLS);
    assert(cell != nullptr);

    if (pos < num_cells) {
        for (auto idx{num_cells}; idx > pos; --idx) {
            cells[idx] = cells[idx - 1];
        }
    }

    cells[pos] = std::move(cell);
    ++num_cells;
    assert(num_cells <= MAX_CELLS);
}

auto Node::insert_child(size_t pos, std::shared_ptr<Node> node) -> void {
    assert(pos < MAX_CHILDREN);
    assert(node != nullptr);

    if (pos < num_children) {
        for (auto idx{num_children}; idx > pos; --idx) {
            children[idx] = children[idx - 1];
        }
    }

    children[pos] = std::move(node);
    ++num_children;
    assert(num_children <= MAX_CHILDREN);
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
    auto mid{MIN_CELLS};
    auto mid_cell{cells[mid]};
    assert(mid_cell != nullptr);

    auto new_node{std::make_shared<Node>()};

    new_node->num_cells = num_cells - (mid + 1);
    assert(new_node->num_cells >= 0);
    for (auto idx{0UL}; idx < new_node->num_cells; ++idx) {
        new_node->cells[idx] = cells[idx + (mid + 1)];
    }
    num_cells = mid;

    if (!is_leaf()) {
        new_node->num_children = num_children - (mid + 1);
        assert(new_node->num_children >= 0);
        for (auto idx{0UL}; idx < new_node->num_children; ++idx) {
            new_node->children[idx] = children[idx + (mid + 1)];
        }
        num_children = mid + 1;
    }

    return std::make_pair(mid_cell, new_node);
}

auto Node::remove_cell(size_t pos) -> std::shared_ptr<Cell> {
    assert(pos < num_cells);
    auto removed{cells[pos]};

    cells[pos] = nullptr;
    auto last{num_cells - 1};
    if (pos < last) {
        for (auto idx{pos}; idx < last; ++idx) {
            cells[idx] = cells[idx + 1];
        }
        cells[last] = nullptr;
    }
    num_cells--;
    assert(num_cells >= 0);

    return removed;
}

auto Node::remove_child(size_t pos) -> std::shared_ptr<Node> {
    assert(pos < num_children);
    auto removed{children[pos]};

    children[pos] = nullptr;
    auto last{num_children - 1};
    if (pos < last) {
        for (auto idx{pos}; idx < last; ++idx) {
            children[idx] = children[idx + 1];
        }
        children[last] = nullptr;
    }
    num_children--;
    assert(num_children >= 0);

    return removed;
}

auto Node::fill_child(size_t pos) -> void {
    if (pos > 0 && children[pos - 1]->num_cells > MIN_CELLS) {
        auto& left{children[pos - 1]};
        auto& right{children[pos]};

        assert(right->num_cells + 1 <= MAX_CELLS);
        for (auto idx{1UL}; idx < right->num_cells + 1; ++idx) {
            right->cells[idx] = right->cells[idx - 1];
        }
        right->cells[0] = cells[pos - 1];
        right->num_cells++;

        if (!right->is_leaf()) {
            right->insert_child(0, left->remove_child(left->num_children - 1));
        }

        cells[pos - 1] = left->remove_cell(left->num_cells - 1);
    } else if (pos < num_children - 1 && children[pos + 1]->num_cells > MIN_CELLS) {
        auto& left{children[pos]};
        auto& right{children[pos + 1]};

        left->cells[left->num_cells] = cells[pos];
        left->num_cells++;

        if (!left->is_leaf()) {
            left->insert_child(left->num_children, right->remove_child(0));
        }

        cells[pos] = right->remove_cell(0);
    } else {
        // NOTE: prefer merging with right sibling for simplicity
        if (pos >= num_cells) {
            pos = num_cells - 1;
        }

        auto& left{children[pos]};
        auto& right{children[pos + 1]};

        left->cells[left->num_cells] = remove_cell(pos);
        left->num_cells++;

        assert(left->num_cells + right->num_cells <= MAX_CELLS);
        assert(left->num_children + right->num_children <= MAX_CHILDREN);

        for (auto idx{0UL}; idx < right->num_cells; ++idx) {
            left->cells[left->num_cells + idx] = right->cells[idx];
        }
        left->num_cells += right->num_cells;

        if (!left->is_leaf()) {
            for (auto idx{0UL}; idx < right->num_children; ++idx) {
                left->children[left->num_children + idx] = right->children[idx];
            }
            left->num_children += right->num_children;
        }

        remove_child(pos + 1);
        right = nullptr;
    }
}

auto Node::remove(size_t key, bool is_seeking_successor) -> std::shared_ptr<Cell> {
    const auto [pos, found]{search(key)};

    Node* next{nullptr};

    if (found) {
        if (is_leaf()) {
            return remove_cell(pos);
        }
        is_seeking_successor = true;
        next = children[pos + 1].get();
    } else {
        next = children[pos].get();
    }

    if (is_leaf() && is_seeking_successor) {
        return remove_cell(0);
    }

    if (next == nullptr) {
        return nullptr;
    }

    // continue traversing
    auto removed_cell{next->remove(key, is_seeking_successor)};

    if (found && is_seeking_successor) {
        cells[pos] = removed_cell;
    }

    if (next->num_cells < MIN_CELLS) {
        if (found && is_seeking_successor) {
            fill_child(pos + 1);
        } else {
            fill_child(pos);
        }
    }

    return removed_cell;
}

auto BTree::show() const -> void {
    if (root) {
        root->show(0);
    } else {
        std::println("EMPTY");
    }
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

auto BTree::remove(size_t key) -> bool {
    if (root == nullptr) {
        return false;
    }

    auto removed_cell{root->remove(key, false)};

    if (root->num_cells == 0) {
        if (root->is_leaf()) {
            root = nullptr;
        } else if (root->num_children == 1) {
            root = root->children[0];
        } else {
            assert(false && "root has multiple children but no keys");
        }
    }

    return removed_cell != nullptr;
}