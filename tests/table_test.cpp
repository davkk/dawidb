#include "table.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
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

        std::array<uint32_t, MAX_NODE_CELLS> indexes{8, 1, 21, 4, 11, 2, 10, 27, 18, 12, 16, 23, 3};

        for (const auto idx : indexes) {
            Row row{.id = idx};

            std::string username = std::format("hello{}", idx);
            std::string email = std::format("world{}", idx);

            std::copy(username.begin(), username.end(), row.username.begin());
            std::copy(email.begin(), email.end(), row.email.begin());

            const auto [result, err]{table.exec({
                StatementType::INSERT,
                {row.id, row.username, row.email},
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
        ASSERT_EQ((*result)[idx].username, rows[idx].username);
        ASSERT_EQ((*result)[idx].email, rows[idx].email);
    }
}
