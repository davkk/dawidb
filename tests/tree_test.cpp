#include "tree.hpp"

#include <gtest/gtest.h>

class BTreeTest : public ::testing::Test {
protected:
    BTree tree;
};

TEST_F(BTreeTest, InsertOneKey) {
    auto key{10UL};
    auto value{20};

    tree.insert(key, value);

    tree.show();

    auto result = tree.find(key);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, value);

    EXPECT_FALSE(tree.find(key + 1).has_value());
}

TEST_F(BTreeTest, InsertMultipleNoSplit) {
    std::vector<Cell> cells{};

    for (auto idx{1UL}; idx <= MAX_CELLS; ++idx) {
        auto key{idx * 10};
        auto value{key + 1};

        cells.emplace_back(key, value);
        tree.insert(key, static_cast<int>(value));
    }

    tree.show();

    for (auto& cell : cells) {
        auto result = tree.find(cell.key);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, cell.value);
    }

    EXPECT_FALSE(tree.find(10 * (MAX_CELLS + 1)).has_value());
}

TEST_F(BTreeTest, InsertCausingSplit) {
    std::vector<Cell> cells{};

    for (auto idx{1UL}; idx <= MAX_CELLS + 1; ++idx) {
        auto key{idx * 10};
        auto value{key + 1};

        cells.emplace_back(key, value);
        tree.insert(key, static_cast<int>(value));
    }

    tree.show();

    for (auto& cell : cells) {
        auto result = tree.find(cell.key);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(*result, cell.value);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::internal::CaptureStderr();
    ::testing::internal::CaptureStdout();

    auto status{RUN_ALL_TESTS()};
    std::println("{}", ::testing::internal::GetCapturedStderr());
    std::println("{}", ::testing::internal::GetCapturedStdout());
    return status;
}