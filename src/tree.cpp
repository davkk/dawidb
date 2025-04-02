#include "tree.hpp"

#include <cassert>
#include <utility>

auto Node::show(size_t level) const -> void {
    std::string indent(level * 4UL, ' ');

    std::print("{}-", indent);
    for (size_t i = 0; i < num_items; ++i) {
        std::print("[{}: {}]", items[i]->key, items[i]->value);
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
    auto high{num_items};

    while (low < high) {
        auto mid{(low + high) / 2UL};
        const auto* item{items[mid].get()};

        if (item->key == key) {
            return {mid, true};
        }

        if (item->key < key) {
            low = mid + 1;
        } else if (item->key > key) {
            high = mid;
        }
    }

    return {low, false};
}

auto Node::insert_item(size_t pos, std::unique_ptr<Item> item) -> void {
    assert(pos < MAX_ITEMS);
    assert(item != nullptr);

    if (pos < num_items) {
        for (auto idx{num_items}; idx > pos; --idx) {
            items[idx] = std::move(items[idx - 1]);
        }
    }

    items[pos] = std::move(item);
    ++num_items;
    assert(num_items <= MAX_ITEMS);
}

auto Node::insert_child(size_t pos, std::unique_ptr<Node> node) -> void {
    assert(pos < MAX_CHILDREN);
    assert(node != nullptr);

    if (pos < num_children) {
        for (auto idx{num_children}; idx > pos; --idx) {
            children[idx] = std::move(children[idx - 1]);
        }
    }

    children[pos] = std::move(node);
    ++num_children;
    assert(num_children <= MAX_CHILDREN);
}

auto Node::insert(Item& item) -> bool {
    auto [pos, found]{search(item.key)};
    auto new_item{std::make_unique<Item>(item)};

    if (found) {
        items[pos] = std::move(new_item);
        return false;
    }

    if (is_leaf()) {
        insert_item(pos, std::move(new_item));
        return true;
    }

    if (children[pos]->num_items >= MAX_ITEMS) {
        auto [mid_item, new_node]{children[pos]->split()};

        insert_item(pos, std::move(mid_item));
        insert_child(pos + 1, std::move(new_node));

        if (item.key > items[pos]->key) {
            pos++;
        } else if (item.key == items[pos]->key) {
            items[pos] = std::move(new_item);
            return true;
        }
    }

    return children[pos]->insert(item);
}

auto Node::split() -> std::pair<std::unique_ptr<Item>, std::unique_ptr<Node>> {
    auto mid{MIN_ITEMS};
    auto new_node{std::make_unique<Node>()};

    assert(num_items >= mid + 1);
    new_node->num_items = num_items - (mid + 1);
    for (auto idx{0UL}; idx < new_node->num_items; ++idx) {
        new_node->items[idx] = std::move(items[idx + (mid + 1)]);
    }
    num_items = mid;

    if (!is_leaf()) {
        assert(num_children >= mid + 1);
        new_node->num_children = num_children - (mid + 1);
        for (auto idx{0UL}; idx < new_node->num_children; ++idx) {
            new_node->children[idx] = std::move(children[idx + (mid + 1)]);
        }
        num_children = mid + 1;
    }

    assert(items[mid] != nullptr);
    return {std::move(items[mid]), std::move(new_node)};
}

auto Node::remove_item(size_t pos) -> std::unique_ptr<Item> {
    assert(pos < num_items);
    auto removed{std::move(items[pos])};

    auto last{num_items - 1};
    if (pos < last) {
        for (auto idx{pos}; idx < last; ++idx) {
            items[idx] = std::move(items[idx + 1]);
        }
        items[last].reset();
    }
    assert(num_items != 0);
    num_items--;

    return removed;
}

auto Node::remove_child(size_t pos) -> std::unique_ptr<Node> {
    assert(pos < num_children);
    auto removed{std::move(children[pos])};

    auto last{num_children - 1};
    if (pos < last) {
        for (auto idx{pos}; idx < last; ++idx) {
            children[idx] = std::move(children[idx + 1]);
        }
        children[last].reset();
    }
    assert(num_children != 0);
    num_children--;

    return removed;
}

auto Node::fill_child(size_t pos) -> void {
    if (pos > 0 && children[pos - 1]->num_items > MIN_ITEMS) {
        auto& left{children[pos - 1]};
        auto& right{children[pos]};

        assert(right->num_items + 1 <= MAX_ITEMS);
        assert(right->items.size() > 1);
        for (auto idx{1UL}; idx < right->num_items + 1; ++idx) {
            right->items[idx] = std::move(right->items[idx - 1]);
        }
        right->items[0] = std::move(items[pos - 1]);
        right->num_items++;

        if (!right->is_leaf()) {
            right->insert_child(0, left->remove_child(left->num_children - 1));
        }

        items[pos - 1] = left->remove_item(left->num_items - 1);
    } else if (pos < num_children - 1 && children[pos + 1]->num_items > MIN_ITEMS) {
        auto& left{children[pos]};
        auto& right{children[pos + 1]};

        left->items[left->num_items] = std::move(items[pos]);
        left->num_items++;

        if (!left->is_leaf()) {
            left->insert_child(left->num_children, right->remove_child(0));
        }

        items[pos] = right->remove_item(0);
    } else {
        // NOTE: prefer merging with right sibling for simplicity
        if (pos >= num_items) {
            pos = num_items - 1;
        }

        auto& left{children[pos]};
        auto& right{children[pos + 1]};

        left->items[left->num_items] = remove_item(pos);
        left->num_items++;

        assert(left->num_items + right->num_items <= MAX_ITEMS);
        assert(left->num_children + right->num_children <= MAX_CHILDREN);

        for (auto idx{0UL}; idx < right->num_items; ++idx) {
            left->items[left->num_items + idx] = std::move(right->items[idx]);
        }
        left->num_items += right->num_items;

        if (!left->is_leaf()) {
            for (auto idx{0UL}; idx < right->num_children; ++idx) {
                left->children[left->num_children + idx] = std::move(right->children[idx]);
            }
            left->num_children += right->num_children;
        }

        remove_child(pos + 1);
        right.reset();
    }
}

auto Node::remove(size_t key, bool is_seeking_successor) -> std::unique_ptr<Item> {
    const auto [pos, found]{search(key)};

    Node* next{nullptr};

    if (found) {
        if (is_leaf()) {
            return remove_item(pos);
        }
        // NOTE: for internal nodes, remove the in-order successor from the right child
        is_seeking_successor = true;
        next = children[pos + 1].get();
    } else {
        next = children[pos].get();
    }

    if (is_leaf() && is_seeking_successor) {
        return remove_item(0);
    }

    if (next == nullptr) {
        return nullptr;
    }

    // continue traversing
    auto removed{next->remove(key, is_seeking_successor)};

    if (found && is_seeking_successor) {
        auto original = std::make_unique<Item>(*items[pos]);
        items[pos] = std::move(removed);
        removed = std::move(original);
    }

    if (next->num_items < MIN_ITEMS) {
        if (found && is_seeking_successor) {
            fill_child(pos + 1);
        } else {
            fill_child(pos);
        }
    }

    return removed;
}

auto BTree::show() const -> void {
    if (root) {
        root->show(0);
    } else {
        std::println("EMPTY");
    }
}

auto BTree::find(size_t key) const -> std::optional<int> {
    auto* curr{root.get()};
    while (curr != nullptr) {
        const auto [pos, found]{curr->search(key)};
        if (found) {
            return curr->items[pos]->value;
        }
        curr = curr->children[pos].get();
    }
    return std::nullopt;
}

auto BTree::split_root() -> void {
    auto new_root{std::make_unique<Node>()};
    auto [item, node]{root->split()};

    new_root->insert_item(0, std::move(item));
    new_root->insert_child(0, std::move(root));
    new_root->insert_child(1, std::move(node));

    root = std::move(new_root);
}

auto BTree::insert(size_t key, int value) -> void {
    auto item{Item{.key = key, .value = value}};

    if (!root) {
        root = std::make_unique<Node>();
    }

    if (root->num_items >= MAX_ITEMS) {
        split_root();
    }

    root->insert(item);
}

auto BTree::remove(size_t key) -> bool {
    if (!root) {
        return false;
    }

    auto removed{root->remove(key, false)};

    if (root->num_items == 0) {
        if (root->is_leaf()) {
            root.reset();
        } else if (root->num_children == 1) {
            root = std::move(root->children[0]);
        } else {
            assert(false && "root has multiple children but no keys");
        }
    }

    return removed != nullptr;
}