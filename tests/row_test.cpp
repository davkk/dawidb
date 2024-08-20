#include "row.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <span>

#include "const.hpp"

TEST(RowTest, SerializeDeserialize) {
    Row initial{420, {"hello"}, {"world"}};

    std::array<char, ROW_SIZE> buffer{};
    initial.serialize_into(std::span<char>{buffer});

    Row final{};
    final.deserialize_from(std::span<char>{buffer});

    ASSERT_EQ(initial.id, final.id);
    ASSERT_EQ(initial.username, final.username);
    ASSERT_EQ(initial.email, final.email);
}
