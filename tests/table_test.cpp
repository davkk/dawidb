#include "table.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <print>
#include <vector>

#include "parser.hpp"
#include "tree.hpp"

class TableTest : public ::testing::Test {
protected:
    std::fstream file;

    void SetUp() override {
        const std::string file_name{"/tmp/test_database"};

        std::ofstream file_new{file_name};
        file_new.close();

        file.open(file_name, std::ios::binary | std::ios::out | std::ios::in);
    }

    void TearDown() override {
        file.close();
        std::remove("/tmp/test_database");
    }
};

TEST_F(TableTest, InsertSaveSelectRead) {
    std::vector<Row> rows;

    {
        Table table{file};

        uint32_t indexes[MAX_NODE_CELLS]{8, 1, 21, 4, 11, 2, 10, 27, 18, 12, 16, 23, 3};

        for (const auto idx : indexes) {
            Row row{.id = idx};

            std::string username = std::format("hello{}", idx);
            std::string email = std::format("world{}", idx);

            std::copy(username.begin(), username.end(), row.username);
            std::copy(email.begin(), email.end(), row.email);

            const auto [result, err]{table.exec({
                StatementType::INSERT,
                row,
            })};

            ASSERT_FALSE("error from insert" && err);
            ASSERT_TRUE("result from insert" && result);

            rows.push_back(row);
        }
    }

    Table table{file};

    const auto [result, err]{table.exec({StatementType::SELECT})};
    ASSERT_FALSE("error from select" && err);
    ASSERT_TRUE("result from select" && result);

    std::ranges::sort(rows, [](const Row& a, const Row& b) {
        return a.id < b.id;
    });

    for (size_t idx{0}; idx < rows.size(); idx++) {
        ASSERT_EQ((*result)[idx].id, rows[idx].id);
        ASSERT_TRUE("usernames equal" && std::strcmp((*result)[idx].username, rows[idx].username) == 0);
        ASSERT_TRUE("emails equal" && std::strcmp((*result)[idx].email, rows[idx].email) == 0);
    }
}
