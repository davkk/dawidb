#include "pager.hpp"

#include <gtest/gtest.h>
#include <unistd.h>

#include <cstdio>
#include <ios>
#include <print>

#include "const.hpp"

class PagerTest : public ::testing::Test {
protected:
    std::fstream file{
        "/tmp/test_database",
        std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc
    };

    void TearDown() override {
        file.close();
        std::remove("/tmp/test_database");
    }
};

TEST_F(PagerTest, ReadEmptyFile) {
    Pager pager{file};
    ASSERT_EQ(pager.file_length, 0);
    ASSERT_TRUE(pager.file.is_open());
}

TEST_F(PagerTest, FindSlotReadTwoPages) {
    auto content{std::string(PAGE_SIZE + ROW_SIZE, 'a')};
    file.write(content.data(), static_cast<std::streamsize>(content.size()));

    Pager pager{file};
    ASSERT_EQ(pager.file_length, PAGE_SIZE + ROW_SIZE);

    for (size_t row{0}; row < ROWS_PER_PAGE * 3; row++) {
        auto [slot, error] = pager.find_slot(row);
        ASSERT_EQ(slot.size(), ROW_SIZE);
        ASSERT_EQ(error, std::nullopt);

        std::string full{std::string(ROW_SIZE, 'a')};
        std::string empty{std::string(ROW_SIZE, '\0')};
        ASSERT_EQ(
            std::string(slot.data(), ROW_SIZE),
            row < ROWS_PER_PAGE + 1 ? full : empty
        );
    }
}

TEST_F(PagerTest, WritePage) {
    auto content{std::string(PAGE_SIZE, 'a')};
    file.write(content.data(), static_cast<std::streamsize>(content.size()));

    Pager pager{file};
    ASSERT_EQ(pager.file_length, PAGE_SIZE);

    auto error = pager.write_page(0, PAGE_SIZE);
    ASSERT_FALSE(error);
    ASSERT_FALSE(file.fail());

    std::string result(PAGE_SIZE, '\0');
    file.seekp(0);
    file.read(result.data(), PAGE_SIZE);
    ASSERT_EQ(content, result);
}
