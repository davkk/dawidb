#include <print>

#include "tree.hpp"

int main() {
    BTree tree;

    tree.insert(10, 10);
    tree.insert(20, 20);
    tree.insert(30, 30);
    tree.insert(40, 40);

    tree.show();

    auto result = tree.find(10);
    if (result.has_value()) {
        std::println("Found: {}", result.value());
    } else {
        std::println("Not found");
    }

    return 0;
}