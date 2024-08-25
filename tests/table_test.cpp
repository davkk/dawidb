#include "table.hpp"

#include <gtest/gtest.h>

#include <format>

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

        for (uint32_t idx{1}; idx <= MAX_NODE_CELLS; idx++) {
            Row row{.id = idx};

            std::string username = std::format("hello{}", idx);
            std::string email = std::format("world{}", idx);

            std::copy(username.begin(), username.end(), row.username.begin());
            std::copy(email.begin(), email.end(), row.email.begin());

            table.exec(
                {StatementType::INSERT, {row.id, row.username, row.email}}
            );
            rows.push_back(row);
        }
    }

    Table table{file};

    auto result{table.exec({StatementType::SELECT})};

    for (size_t idx{0}; idx < MAX_NODE_CELLS; idx++) {
        ASSERT_EQ(result[idx].id, rows[idx].id);
        ASSERT_EQ(result[idx].username, rows[idx].username);
        ASSERT_EQ(result[idx].email, rows[idx].email);
    }
}
