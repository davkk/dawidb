#pragma once

#include <memory>
#include <print>

constexpr auto PAGE_SIZE = 4096UL;
constexpr auto MAX_PAGES = 100UL;

struct Item {
    size_t key;
    int value;
};

constexpr auto DEGREE = 3UL;
constexpr auto MAX_CHILDREN = 2UL * DEGREE;
constexpr auto MAX_ITEMS = MAX_CHILDREN - 1UL;
constexpr auto MIN_ITEMS = DEGREE - 1UL;

struct Node {
    size_t num_items{0};
    size_t num_children{0};

    std::array<std::unique_ptr<Item>, MAX_ITEMS> items;
    std::array<std::unique_ptr<Node>, MAX_CHILDREN> children;

    auto show(size_t level) const -> void;
    [[nodiscard]] auto is_leaf() const -> bool;
    auto search(size_t key) -> std::pair<size_t, bool>;
    auto insert_item(size_t pos, std::unique_ptr<Item> item) -> void;
    auto insert_child(size_t pos, std::unique_ptr<Node> node) -> void;
    auto insert(Item& item) -> bool;
    auto split() -> std::pair<std::unique_ptr<Item>, std::unique_ptr<Node>>;
    auto remove_item(size_t pos) -> std::unique_ptr<Item>;
    auto remove_child(size_t pos) -> std::unique_ptr<Node>;
    auto fill_child(size_t pos) -> void;
    auto remove(size_t key, bool is_seeking_successor) -> std::unique_ptr<Item>;
};

struct BTree {
    std::unique_ptr<Node> root{nullptr};

    auto show() const -> void;
    [[nodiscard]] auto find(size_t key) const -> std::optional<int>;
    auto split_root() -> void;
    auto insert(size_t key, int value) -> void;
    auto remove(size_t key) -> bool;
};