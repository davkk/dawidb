#include "tree.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <vector>

class BTreeTest : public ::testing::Test {
protected:
    BTree tree;
    std::vector<Item> items;

    void insert_keys(const std::vector<size_t>& keys) {
        items.clear();  // ensure items is reset for each test
        for (auto key : keys) {
            auto value{static_cast<int>(key * 10)};
            items.emplace_back(key, value);
            tree.insert(key, value);
        }
    }
};

TEST_F(BTreeTest, InsertOneKey) {
    auto key{10UL};
    insert_keys({key});

    tree.show();

    auto result = tree.find(key);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, key * 10);

    EXPECT_FALSE(tree.find(key + 1).has_value());
}

TEST_F(BTreeTest, InsertMultipleNoSplit) {
    std::vector<size_t> keys;
    for (auto idx{1UL}; idx <= MAX_ITEMS; ++idx) {
        keys.push_back(idx * 10);
    }
    insert_keys(keys);

    tree.show();

    for (const auto& item : items) {
        auto result = tree.find(item.key);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, item.value);
    }

    EXPECT_FALSE(tree.find(10 * (MAX_ITEMS + 1)).has_value());
}

TEST_F(BTreeTest, InsertCausingSplit) {
    std::vector<size_t> keys;
    for (auto idx{1UL}; idx <= MAX_ITEMS + 1; ++idx) {
        keys.push_back(idx * 10);
    }
    insert_keys(keys);

    tree.show();

    for (const auto& item : items) {
        auto result = tree.find(item.key);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, item.value);
    }
}

TEST_F(BTreeTest, RemoveExistingKey) {
    auto key{30UL};
    auto value{40};
    items.emplace_back(key, value);
    tree.insert(key, value);

    tree.show();

    auto result = tree.find(key);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, value);

    bool removed = tree.remove(key);
    EXPECT_TRUE(removed);

    tree.show();
    auto resultAfterRemoval = tree.find(key);
    EXPECT_FALSE(resultAfterRemoval.has_value());
}

TEST_F(BTreeTest, RemoveNonExistentKey) {
    auto key{50UL};
    auto value{60};
    items.emplace_back(key, value);
    tree.insert(key, value);

    tree.show();

    bool removed = tree.remove(key + 10);
    EXPECT_FALSE(removed);

    auto result = tree.find(key);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, value);
}

TEST_F(BTreeTest, InsertDuplicateKey) {
    auto key{70UL};
    auto value1{80};
    auto value2{90};

    items.emplace_back(key, value1);
    tree.insert(key, value1);
    tree.show();

    items.clear();  // Update expected value for duplicate
    items.emplace_back(key, value2);
    tree.insert(key, value2);
    tree.show();

    auto result = tree.find(key);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, value2);
}

TEST_F(BTreeTest, BorrowFromLeftSibling) {
    std::vector<size_t> keys{10, 20, 30, 40, 50};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(30);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_TRUE(tree.find(10).has_value());
    EXPECT_EQ(*tree.find(10), 100);
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_FALSE(tree.find(30).has_value());
    EXPECT_TRUE(tree.find(40).has_value());
    EXPECT_EQ(*tree.find(40), 400);
    EXPECT_TRUE(tree.find(50).has_value());
    EXPECT_EQ(*tree.find(50), 500);
}

TEST_F(BTreeTest, BorrowFromRightSibling) {
    std::vector<size_t> keys{10, 20, 30, 40, 50};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(10);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_FALSE(tree.find(10).has_value());
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_TRUE(tree.find(30).has_value());
    EXPECT_EQ(*tree.find(30), 300);
    EXPECT_TRUE(tree.find(40).has_value());
    EXPECT_EQ(*tree.find(40), 400);
    EXPECT_TRUE(tree.find(50).has_value());
    EXPECT_EQ(*tree.find(50), 500);
}

TEST_F(BTreeTest, MergeWithLeftSibling) {
    std::vector<size_t> keys{10, 20, 30};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(20);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_TRUE(tree.find(10).has_value());
    EXPECT_EQ(*tree.find(10), 100);
    EXPECT_FALSE(tree.find(20).has_value());
    EXPECT_TRUE(tree.find(30).has_value());
    EXPECT_EQ(*tree.find(30), 300);
}

TEST_F(BTreeTest, MergeWithRightSibling) {
    std::vector<size_t> keys{10, 20, 30};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(10);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_FALSE(tree.find(10).has_value());
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_TRUE(tree.find(30).has_value());
    EXPECT_EQ(*tree.find(30), 300);
}

TEST_F(BTreeTest, NoLeftSibling) {
    std::vector<size_t> keys{10, 20, 30, 40};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(10);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_FALSE(tree.find(10).has_value());
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_TRUE(tree.find(30).has_value());
    EXPECT_EQ(*tree.find(30), 300);
    EXPECT_TRUE(tree.find(40).has_value());
    EXPECT_EQ(*tree.find(40), 400);
}

TEST_F(BTreeTest, NoRightSibling) {
    std::vector<size_t> keys{10, 20, 30, 40};
    insert_keys(keys);

    tree.show();

    bool removed = tree.remove(40);
    EXPECT_TRUE(removed);

    tree.show();

    EXPECT_TRUE(tree.find(10).has_value());
    EXPECT_EQ(*tree.find(10), 100);
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_TRUE(tree.find(30).has_value());
    EXPECT_EQ(*tree.find(30), 300);
    EXPECT_FALSE(tree.find(40).has_value());
}

TEST_F(BTreeTest, MultiLevelTree) {
    std::vector<size_t> keys{10, 20, 30, 40, 50, 60, 70, 80, 90};
    insert_keys(keys);

    tree.show();

    bool removed1 = tree.remove(30);
    EXPECT_TRUE(removed1);
    bool removed2 = tree.remove(40);
    EXPECT_TRUE(removed2);

    tree.show();

    EXPECT_TRUE(tree.find(10).has_value());
    EXPECT_EQ(*tree.find(10), 100);
    EXPECT_TRUE(tree.find(20).has_value());
    EXPECT_EQ(*tree.find(20), 200);
    EXPECT_FALSE(tree.find(30).has_value());
    EXPECT_FALSE(tree.find(40).has_value());
    EXPECT_TRUE(tree.find(50).has_value());
    EXPECT_EQ(*tree.find(50), 500);
    EXPECT_TRUE(tree.find(60).has_value());
    EXPECT_EQ(*tree.find(60), 600);
    EXPECT_TRUE(tree.find(70).has_value());
    EXPECT_EQ(*tree.find(70), 700);
    EXPECT_TRUE(tree.find(80).has_value());
    EXPECT_EQ(*tree.find(80), 800);
    EXPECT_TRUE(tree.find(90).has_value());
    EXPECT_EQ(*tree.find(90), 900);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::internal::CaptureStderr();
    ::testing::internal::CaptureStdout();

    auto status{RUN_ALL_TESTS()};
    std::cout << ::testing::internal::GetCapturedStderr();
    std::cout << ::testing::internal::GetCapturedStdout();
    return status;
}